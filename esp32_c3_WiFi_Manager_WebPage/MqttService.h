#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <Arduino.h>
#include "MqttManager.h"
#include "MqttCredentials.h"
#include "StateManager.h"

// Тип колбэка для обработки входящих MQTT-сообщений
typedef std::function<void(const String& topic, const String& payload)> MqttMessageCallback;

// === ИЗМЕНЕНИЕ: тип колбэка для события успешного (пере)подключения к брокеру ===
typedef std::function<void()> MqttConnectedCallback;
// === КОНЕЦ ИЗМЕНЕНИЯ ===

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

    // === ИЗМЕНЕНИЕ: установка внешнего колбэка на успешное (пере)подключение ===
    void setOnConnected(MqttConnectedCallback callback);
    // === КОНЕЦ ИЗМЕНЕНИЯ ===

private:
    StateManager& _stateManager;
    MqttManager _manager;
    MqttCredentials _credentials;
    MqttMessageCallback _messageCallback;

    // === ИЗМЕНЕНИЕ: храним внешний колбэк (пере)подключения ===
    MqttConnectedCallback _connectedCallback;
    // === КОНЕЦ ИЗМЕНЕНИЯ ===

    // Приватный обработчик (вызывается из статического колбэка MqttManager)
    void handleMessage(const String& topic, const String& payload);
};

#endif