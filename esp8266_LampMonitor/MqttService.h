#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <Arduino.h>
#include "MqttManager.h"
#include "MqttCredentials.h"
#include "StateManager.h"

typedef std::function<void(const String& topic, const String& payload)> MqttMessageCallback;
typedef std::function<void()> MqttConnectedCallback;

class MqttService {
public:
    MqttService(StateManager& stateManager);

    bool begin();
    // === ИЗМЕНЕНО: сигнатуры без cmdTopic ===
    bool begin(const String& server, int port,
               const String& user, const String& password,
               const String& stateTopic);
    bool configure(const String& server, int port,
                   const String& user, const String& password,
                   const String& stateTopic);
    bool saveCredentials();
    bool loadCredentials();
    MqttCredentials& getCredentials() { return _credentials; }

    void update();

    // === УДАЛЕНО: publishState — только чтение ===

    bool isConnected();

    void setMessageCallback(MqttMessageCallback callback);
    void setOnConnected(MqttConnectedCallback callback);

    bool subscribeState();

private:
    StateManager& _stateManager;
    MqttManager _manager;
    MqttCredentials _credentials;
    MqttMessageCallback _messageCallback;
    MqttConnectedCallback _connectedCallback;

    void handleMessage(const String& topic, const String& payload);
};

#endif