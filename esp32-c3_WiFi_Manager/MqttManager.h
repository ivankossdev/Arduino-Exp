#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

typedef std::function<void(const String& topic, const String& payload)> MqttCallback;

class MqttManager {
public:
    MqttManager();
    ~MqttManager() = default;

    bool begin(const String& server, int port,
               const String& user, const String& password,
               const String& cmdTopic, const String& stateTopic,
               MqttCallback callback = nullptr);

    void update();

    bool publishState(const String& message, bool retained = true);

    bool isConnected();   // убрали const

    void reconnect();

private:
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;

    String _server;
    int _port;
    String _user;
    String _password;
    String _cmdTopic;
    String _stateTopic;

    MqttCallback _callback;

    static void staticCallback(char* topic, byte* payload, unsigned int length);
    void internalCallback(const String& topic, const String& payload);
};

#endif