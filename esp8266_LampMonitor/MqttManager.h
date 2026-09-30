#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <functional>

typedef std::function<void(const String& topic, const String& payload)> MqttCallback;
typedef std::function<void()> MqttConnectedCallback;

class MqttManager {
public:
    MqttManager();
    ~MqttManager() = default;

    bool begin(const String& server, int port,
               const String& user, const String& password,
               const String& stateTopic);

    // === НОВОЕ: чистый разрыв соединения ===
    // Нужен перед повторной инициализацией (например, когда через веб
    // меняются настройки MQTT). Без этого PubSubClient может остаться
    // «подключённым» к старому брокеру со старыми подписками.
    void disconnect();

    static void setCallback(MqttCallback callback);
    static void setOnConnected(MqttConnectedCallback callback);

    void update();

    bool isConnected();

    bool subscribe(const String& topic);

    void reconnect();

private:
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;

    String _server;
    int _port;
    String _user;
    String _password;
    String _stateTopic;

    static MqttCallback _callback;
    static MqttConnectedCallback _onConnected;

    static void staticCallback(char* topic, byte* payload, unsigned int length);
};

#endif