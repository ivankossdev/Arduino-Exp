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

    // === НОВОЕ: флаг, что статические колбэки уже зарегистрированы ===
    // MqttManager::setCallback и setOnConnected — статические, и их повторная
    // регистрация при каждом begin() может приводить к нежелательным эффектам
    // (перезапись лямбд, двойная подписка, дублирование сообщений).
    // Регистрируем один раз — при первом успешном begin().
    bool _callbacksRegistered = false;

    void handleMessage(const String& topic, const String& payload);
};

#endif