#include "MqttManager.h"

MqttCallback MqttManager::_callback = nullptr;
MqttConnectedCallback MqttManager::_onConnected = nullptr;

MqttManager::MqttManager()
    : _port(1883), _mqttClient(_wifiClient) {}

// === ИЗМЕНЕНО: убран cmdTopic, убрана подписка на cmdTopic ===
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
    static int attempts = 0;
    static unsigned long lastAttemptTime = 0;
    const int MAX_ATTEMPTS = 10;
    const unsigned long BASE_DELAY = 1000;

    if (attempts > 0 && millis() - lastAttemptTime < (BASE_DELAY << (attempts - 1))) {
        return;
    }

    if (attempts >= MAX_ATTEMPTS) {
        Serial.println("❌ MQTT: превышено максимальное число попыток.");
        return;
    }

    if (WiFi.status() != WL_CONNECTED) {
        attempts = 0;
        return;
    }

    Serial.print("Попытка подключения к MQTT... (");
    Serial.print(attempts + 1);
    Serial.print("/");
    Serial.print(MAX_ATTEMPTS);
    Serial.println(")");

    String clientId = "ESP8266_Client_" + String(random(0xffff), HEX);
    if (_mqttClient.connect(clientId.c_str(), _user.c_str(), _password.c_str())) {
        Serial.println(" ✅ MQTT подключён");

        // === УДАЛЕНО: подписка на cmdTopic (устройство только читает) ===
        // === ОСТАВЛЕНО: уведомление о подключении — LampStateService подпишется сам ===

        if (_onConnected) {
            _onConnected();
        }

        attempts = 0;
        lastAttemptTime = 0;
    } else {
        Serial.printf(" ❌ MQTT fail, rc=%d\n", _mqttClient.state());
        attempts++;
        lastAttemptTime = millis();
        unsigned long delayMs = BASE_DELAY << (attempts - 1);
        if (delayMs > 60000) delayMs = 60000;
        Serial.printf("Следующая попытка через %lu сек\n", delayMs / 1000);
    }
}

// === УДАЛЕНО: publishState ===