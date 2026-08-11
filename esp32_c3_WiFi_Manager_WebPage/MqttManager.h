#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <functional>

typedef std::function<void(const String& topic, const String& payload)> MqttCallback;

class MqttManager {
public:
    MqttManager();
    ~MqttManager() = default;

    bool begin(const String& server, int port,
               const String& user, const String& password,
               const String& cmdTopic, const String& stateTopic);

    static void setCallback(MqttCallback callback);

    void update();

    bool publishState(const String& message, bool retained = true);
    bool isConnected();  

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

    static MqttCallback _callback;

    static void staticCallback(char* topic, byte* payload, unsigned int length);
    void internalCallback(const String& topic, const String& payload);
};

#endif