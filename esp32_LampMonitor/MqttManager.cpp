// ============================================================
// MqttManager — реализация для ESP32
// ============================================================
// ИЗМЕНЕНО: buffer size, backoff и задержки берутся из config.h
// ИЗМЕНЕНО: префикс clientId берётся из config.h
// ============================================================

#include "MqttManager.h"
#include "config.h"      // ИЗМЕНЕНО: подключены константы

MqttCallback MqttManager::_callback = nullptr;
MqttConnectedCallback MqttManager::_onConnected = nullptr;

MqttManager::MqttManager()
    // ИЗМЕНЕНО: было _port(1883)
    : _port(MQTT_DEFAULT_PORT), _mqttClient(_wifiClient) {}

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
    // ИЗМЕНЕНО: было setBufferSize(1024)
    _mqttClient.setBufferSize(MQTT_BUFFER_SIZE);
    return true;
}

void MqttManager::disconnect() {
    if (_mqttClient.connected()) {
        _mqttClient.disconnect();
        // ИЗМЕНЕНО: было delay(50)
        delay(MQTT_DISCONNECT_DELAY_MS);
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
    static unsigned long lastAttemptTime = 0;
    static unsigned long currentDelay = 0;

    // ИЗМЕНЕНО: было BASE_DELAY = 1000, MAX_DELAY = 60000
    const unsigned long BASE_DELAY = MQTT_RECONNECT_BASE_DELAY_MS;
    const unsigned long MAX_DELAY  = MQTT_RECONNECT_MAX_DELAY_MS;

    if (WiFi.status() != WL_CONNECTED) {
        lastAttemptTime = 0;
        currentDelay = 0;
        return;
    }

    if (currentDelay > 0 && millis() - lastAttemptTime < currentDelay) {
        return;
    }

    // ИЗМЕНЕНО: префикс "ESP32_Client_" берётся из config.h
    String clientId = String(MQTT_CLIENT_ID_PREFIX) + String(random(0xffff), HEX);

    if (_mqttClient.connect(clientId.c_str(), _user.c_str(), _password.c_str())) {
        Serial.println("[MQTT] Подключено");
        if (_onConnected) _onConnected();
        lastAttemptTime = 0;
        currentDelay = 0;
    } else {
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