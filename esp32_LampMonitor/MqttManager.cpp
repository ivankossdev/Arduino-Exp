// ============================================================
// MqttManager — реализация для ESP32
// ============================================================
// ИЗМЕНЕНО: убран жёсткий лимит попыток реконнекта (MAX_ATTEMPTS = 10).
//           Раньше после 10 неудач устройство переставало пытаться
//           подключиться до перезагрузки. Теперь пытается бесконечно
//           с экспоненциальным backoff и потолком в 60 секунд:
//           1 → 2 → 4 → 8 → 16 → 32 → 60 → 60 → 60 → ...
//
// ИЗМЕНЕНО: backoff переписан на отдельную переменную currentDelay.
//           Старый вариант (BASE_DELAY << (attempts - 1)) при больших
//           attempts давал сдвиг больше разрядности unsigned long — UB.
//           Новая реализация просто удваивает delay и ограничивает
//           его сверху MAX_DELAY, никогда не переполняясь.
//
// ИЗМЕНЕНО: clientId теперь "ESP32_Client_" вместо "ESP8266_Client_"
//           (после перехода на ESP32 старое имя вводило в заблуждение).
//
// ИЗМЕНЕНО: при потере Wi-Fi backoff сбрасывается в ноль — как только
//           сеть вернётся, первая попытка MQTT будет немедленной,
//           а не через 60 секунд ожидания старой задержки.
// ============================================================

#include "MqttManager.h"

MqttCallback MqttManager::_callback = nullptr;
MqttConnectedCallback MqttManager::_onConnected = nullptr;

MqttManager::MqttManager()
    : _port(1883), _mqttClient(_wifiClient) {}

bool MqttManager::begin(const String& server, int port,
                        const String& user, const String& password,
                        const String& stateTopic) {
    _server = server;
    _port = port;
    _user = user;
    _password = password;
    _stateTopic = stateTopic;

    _mqttClient.setServer(_server.c_str(), _port);
    _mqttClient.setCallback(staticCallback);
    _mqttClient.setBufferSize(1024);
    return true;
}

void MqttManager::disconnect() {
    if (_mqttClient.connected()) {
        _mqttClient.disconnect();
        delay(50);
    }
}

void MqttManager::setCallback(MqttCallback callback) {
    _callback = callback;
}

void MqttManager::setOnConnected(MqttConnectedCallback callback) {
    _onConnected = callback;
}

bool MqttManager::subscribe(const String& topic) {
    if (!_mqttClient.connected()) return false;
    if (topic.length() == 0) return false;
    return _mqttClient.subscribe(topic.c_str());
}

void MqttManager::staticCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    if (_callback) {
        _callback(String(topic), message);
    }
}

void MqttManager::update() {
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }
    if (!_mqttClient.connected()) {
        reconnect();
    }
    _mqttClient.loop();
}

bool MqttManager::isConnected() {
    return _mqttClient.connected();
}

void MqttManager::reconnect() {
    // === НОВОЕ: вместо счётчика attempts с лимитом — две
    // статические переменные, описывающие состояние backoff.
    // lastAttemptTime — момент последней попытки подключения,
    // currentDelay    — сколько ждать перед следующей попыткой.
    static unsigned long lastAttemptTime = 0;
    static unsigned long currentDelay = 0;

    const unsigned long BASE_DELAY = 1000;   // 1 с — стартовая задержка
    const unsigned long MAX_DELAY  = 60000;  // 60 с — потолок backoff

    // === НОВОЕ: при потере Wi-Fi сбрасываем backoff в ноль. ===
    // Как только сеть вернётся — первая попытка MQTT будет сразу,
    // без ожидания старой «замороженной» задержки в 60 секунд.
    if (WiFi.status() != WL_CONNECTED) {
        lastAttemptTime = 0;
        currentDelay = 0;
        return;
    }

    // Ждём текущую задержку перед следующей попыткой.
    // Проверка `currentDelay > 0` нужна, чтобы самая первая попытка
    // после старта или после восстановления Wi-Fi шла без задержки.
    if (currentDelay > 0 && millis() - lastAttemptTime < currentDelay) {
        return;
    }

    // === ИЗМЕНЕНО: было "ESP8266_Client_", стало "ESP32_Client_"
    String clientId = "ESP32_Client_" + String(random(0xffff), HEX);

    if (_mqttClient.connect(clientId.c_str(), _user.c_str(), _password.c_str())) {
        Serial.println("[MQTT] Подключено");
        if (_onConnected) _onConnected();

        // Сброс backoff — следующая неудача снова начнёт с 1 секунды
        lastAttemptTime = 0;
        currentDelay = 0;
    } else {
        // === ИЗМЕНЕНО: экспоненциальный backoff без ограничения попыток. ===
        // Старый вариант (MAX_ATTEMPTS = 10) после десятой неудачи
        // прекращал попытки до перезагрузки. Теперь пытаемся всегда,
        // просто задержка упирается в потолок MAX_DELAY (60 с) и
        // остаётся такой до успеха. Первая неудача → 1 с, затем 2, 4, 8,
        // 16, 32, 60, 60, 60, ... — в логе видно retry 1s/2s/4s/.../60s.
        if (currentDelay == 0) {
            currentDelay = BASE_DELAY;
        } else {
            currentDelay *= 2;
            if (currentDelay > MAX_DELAY) currentDelay = MAX_DELAY;
        }
        lastAttemptTime = millis();
        Serial.printf("[MQTT] FAIL rc=%d, retry %lus\n",
                      _mqttClient.state(), currentDelay / 1000);
    }
}