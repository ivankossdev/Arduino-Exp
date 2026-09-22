#include "MqttManager.h"

MqttCallback MqttManager::_callback = nullptr;

// === ИЗМЕНЕНИЕ: определение статического колбэка (пере)подключения ===
MqttConnectedCallback MqttManager::_onConnected = nullptr;
// === КОНЕЦ ИЗМЕНЕНИЯ ===

MqttManager::MqttManager()
    : _port(1883), _mqttClient(_wifiClient) {}

bool MqttManager::begin(const String& server, int port,
                        const String& user, const String& password,
                        const String& cmdTopic, const String& stateTopic) {
    _server = server;
    _port = port;
    _user = user;
    _password = password;
    _cmdTopic = cmdTopic;
    _stateTopic = stateTopic;

    _mqttClient.setServer(_server.c_str(), _port);
    _mqttClient.setCallback(staticCallback);
    _mqttClient.setBufferSize(1024);
    return true;
}

void MqttManager::setCallback(MqttCallback callback) {
    _callback = callback;
}

// === ИЗМЕНЕНИЕ: реализация регистрации колбэка (пере)подключения ===
void MqttManager::setOnConnected(MqttConnectedCallback callback) {
    _onConnected = callback;
}
// === КОНЕЦ ИЗМЕНЕНИЯ ===

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

// --- ИСПРАВЛЕННЫЙ МЕТОД ПЕРЕПОДКЛЮЧЕНИЯ ---
void MqttManager::reconnect() {
    static int attempts = 0;
    static unsigned long lastAttemptTime = 0;
    const int MAX_ATTEMPTS = 10;
    const unsigned long BASE_DELAY = 1000;

    if (attempts > 0 && millis() - lastAttemptTime < (BASE_DELAY << (attempts - 1))) {
        return;
    }

    if (attempts >= MAX_ATTEMPTS) {
        Serial.println("❌ MQTT: превышено максимальное число попыток переподключения.");
        return;
    }

    if (WiFi.status() != WL_CONNECTED) {
        attempts = 0;
        return;
    }

    Serial.print("Попытка подключения к MQTT брокеру... (попытка ");
    Serial.print(attempts + 1);
    Serial.print(" из ");
    Serial.print(MAX_ATTEMPTS);
    Serial.println(")");

    String clientId = "ESP32_Client_" + String(random(0xffff), HEX);
    if (_mqttClient.connect(clientId.c_str(), _user.c_str(), _password.c_str())) {
        Serial.println(" УСПЕШНО!");
        if (_mqttClient.subscribe(_cmdTopic.c_str())) {
            Serial.printf("Подписались на топик: %s\n", _cmdTopic.c_str());
        } else {
            Serial.println("Ошибка подписки на топик!");
        }

        // === ИЗМЕНЕНИЕ: вместо принудительной публикации "OFF" уведомляем подписчика ===
        // Раньше здесь было:
        //     publishState("OFF", true);
        // Из-за этого при каждом реконнекте брокер получал "OFF" независимо от
        // фактического состояния лампы, и состояние на брокере рассинхронизировалось
        // с реальным. Теперь MqttManager не навязывает состояние — он только
        // сообщает наружу о факте подключения, а актуальное состояние публикует
        // тот, кто реально им управляет (AppState).
        if (_onConnected) {
            _onConnected();
        }
        // === КОНЕЦ ИЗМЕНЕНИЯ ===

        attempts = 0;
        lastAttemptTime = 0;
    } else {
        Serial.printf(" Провал, rc=%d\n", _mqttClient.state());
        attempts++;
        lastAttemptTime = millis();
        unsigned long delayMs = BASE_DELAY << (attempts - 1);
        if (delayMs > 60000) delayMs = 60000;
        Serial.printf("Следующая попытка через %lu секунд\n", delayMs / 1000);
    }
}

bool MqttManager::publishState(const String& message, bool retained) {
    if (!_mqttClient.connected()) return false;
    return _mqttClient.publish(_stateTopic.c_str(), message.c_str(), retained);
}