#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <Arduino.h>
#include "MqttManager.h"
#include "MqttCredentials.h"
#include "StateManager.h"

// Тип колбэка для обработки входящих MQTT-сообщений
typedef std::function<void(const String& topic, const String& payload)> MqttMessageCallback;

class MqttService {
public:
    MqttService(StateManager& stateManager);

    // --- Инициализация ---
    bool begin(); // использует сохранённые настройки
    bool begin(const String& server, int port,
               const String& user, const String& password,
               const String& cmdTopic, const String& stateTopic);
    bool configure(const String& server, int port,
                   const String& user, const String& password,
                   const String& cmdTopic, const String& stateTopic);
    bool saveCredentials();
    bool loadCredentials();
    MqttCredentials& getCredentials() { return _credentials; }

    // --- Обновление (вызывается из loop) ---
    void update();

    // --- Публикация ---
    bool publishState(const String& message, bool retained = true);

    // --- Статус ---
    bool isConnected();

    // --- Установка колбэка для входящих сообщений ---
    void setMessageCallback(MqttMessageCallback callback);

private:
    StateManager& _stateManager;
    MqttManager _manager;
    MqttCredentials _credentials;
    MqttMessageCallback _messageCallback;

    // Приватный обработчик (вызывается из статического колбэка MqttManager)
    void handleMessage(const String& topic, const String& payload);
};

#endif