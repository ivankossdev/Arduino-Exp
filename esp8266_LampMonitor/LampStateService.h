#ifndef LAMP_STATE_SERVICE_H
#define LAMP_STATE_SERVICE_H

#include <Arduino.h>
#include <functional>
#include "MqttService.h"

// === НОВЫЙ КЛАСС ===
// Заменяет LedService. Устройство — только читатель:
//  - при (пере)подключении к MQTT подписывается на state-топик;
//  - принимает входящие сообщения и хранит последний payload;
//  - уведомляет подписчиков (DisplayService) через колбэк.
//
// Никакой публикации, никакой синхронизации retained-ON/OFF —
// брокер всегда прав, что пришло, то и показываем.

typedef std::function<void(const String& payload)> LampStateCallback;

class LampStateService {
public:
    explicit LampStateService(MqttService& mqttService);

    // Колбэк на каждое новое значение (для мгновенной перерисовки дисплея)
    void setOnStateChange(LampStateCallback cb) { _onChange = cb; }

    // Вызывается из AppState::handleMqttMessage.
    // true — сообщение относится к нашему топику.
    bool handleMqttMessage(const String& topic, const String& payload);

    // Геттеры
    const String& getPayload() const { return _payload; }
    bool hasData() const { return _hasData; }

    // Событие (пере)подключения к MQTT — вызывается MqttService
    void onMqttConnected();

private:
    MqttService& _mqttService;
    String _payload;
    bool _hasData = false;
    LampStateCallback _onChange;
};

#endif