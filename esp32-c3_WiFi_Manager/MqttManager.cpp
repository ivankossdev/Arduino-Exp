#include "MqttManager.h"

MqttCallback MqttManager::_callback = nullptr;

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

bool MqttManager::isConnected() {   // убрали const
    return _mqttClient.connected();
}

void MqttManager::reconnect() {
    static int attempts = 0;
    while (!_mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
        Serial.print("Попытка подключения к MQTT брокеру...");
        String clientId = "ESP32_Client_" + String(random(0xffff), HEX);
        if (_mqttClient.connect(clientId.c_str(), _user.c_str(), _password.c_str())) {
            Serial.println(" УСПЕШНО!");
            if (_mqttClient.subscribe(_cmdTopic.c_str())) {
                Serial.printf("Подписались на топик: %s\n", _cmdTopic.c_str());
            } else {
                Serial.println("Ошибка подписки на топик!");
            }
            publishState("OFF", true);
            attempts = 0;
        } else {
            Serial.printf(" Провал, rc=%d, попытка через 5 сек\n", _mqttClient.state());
            attempts++;
            if (attempts > 5) {
                Serial.println("Слишком много неудачных попыток переподключения. Проверьте настройки MQTT.");
                attempts = 0;
                delay(5000);
                break;
            }
            delay(5000);
        }
    }
}

bool MqttManager::publishState(const String& message, bool retained) {
    if (!_mqttClient.connected()) return false;
    return _mqttClient.publish(_stateTopic.c_str(), message.c_str(), retained);
}