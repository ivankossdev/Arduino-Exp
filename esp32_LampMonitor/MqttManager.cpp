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
    static int attempts = 0;
    static unsigned long lastAttemptTime = 0;
    const int MAX_ATTEMPTS = 10;
    const unsigned long BASE_DELAY = 1000;

    if (attempts > 0 && millis() - lastAttemptTime < (BASE_DELAY << (attempts - 1))) {
        return;
    }

    if (attempts >= MAX_ATTEMPTS) {
        Serial.println("[MQTT] Превышен лимит попыток — до перезагрузки");
        return;
    }

    if (WiFi.status() != WL_CONNECTED) {
        attempts = 0;
        return;
    }

    String clientId = "ESP8266_Client_" + String(random(0xffff), HEX);
    if (_mqttClient.connect(clientId.c_str(), _user.c_str(), _password.c_str())) {
        // === ИЗМЕНЕНО: один короткий лог на успех ===
        Serial.println("[MQTT] Подключено");
        if (_onConnected) _onConnected();
        attempts = 0;
        lastAttemptTime = 0;
    } else {
        // === ИЗМЕНЕНО: один лог на провал с rc и временем до следующей попытки ===
        attempts++;
        lastAttemptTime = millis();
        unsigned long delayMs = BASE_DELAY << (attempts - 1);
        if (delayMs > 60000) delayMs = 60000;
        Serial.printf("[MQTT] FAIL rc=%d, retry %lus\n",
                      _mqttClient.state(), delayMs / 1000);
    }
}