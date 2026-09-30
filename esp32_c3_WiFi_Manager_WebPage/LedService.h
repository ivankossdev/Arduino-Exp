#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include <Arduino.h>
#include "LedManager.h"
#include "MqttService.h"

// === НОВЫЙ КЛАСС (шаги 2 и 3 рефакторинга AppState) ===
// Инкапсулирует:
//   - низкоуровневое управление светодиодом через LedManager (шаг 2);
//   - публикацию состояния в MQTT при локальных изменениях (веб, кнопка) — шаг 2;
//   - синхронизацию с retained-состоянием при (пере)подключении к брокеру (шаг 3).
//
// Раньше всё это жило в AppState: методы beginLed/setLed/updateLed/getLedState,
// поля _awaitingRetainedState/_connectedAt/_retainedWaitTimeout, а также часть
// AppState::handleMqttMessage и AppState::update().
//
// Теперь AppState только маршрутизирует сюда входящие MQTT-сообщения и дергает
// update() в loop().
class LedService {
public:
    explicit LedService(MqttService& mqttService);

    // --- Инициализация ---
    bool begin(int pin, bool activeLow = true);
    void update();

    // --- Локальные команды (веб-интерфейс, кнопка и т.п.) ---
    // set() сам публикует новое состояние в MQTT, если брокер подключён.
    void set(bool on);
    bool isOn() const;

    // --- Обработка входящих MQTT-сообщений ---
    // Возвращает true, если сообщение относится к светодиоду
    // (retained-state из state-топика либо команда из cmd-топика).
    // Возврат false означает, что сообщение не наше и AppState может
    // попробовать другие обработчики.
    bool handleMqttMessage(const String& topic, const String& payload);

    // --- Событие (пере)подключения к брокеру ---
    // Вызывается MqttService через свой onConnected-колбэк (зарегистрирован
    // в конструкторе LedService).
    void onMqttConnected();

private:
    MqttService& _mqttService;
    LedManager _led;

    // --- Состояние retained-синхронизации (шаг 3) ---
    // _awaitingRetained — true, пока после (пере)подключения ждём retained-сообщение.
    // _connectedAt     — момент последнего (пере)подключения (для таймаута).
    // _retainedTimeout — сколько ждать retained, прежде чем опубликовать своё состояние.
    bool _awaitingRetained = false;
    unsigned long _connectedAt = 0;
    const unsigned long _retainedTimeout = 1500;

    // Применяет retained-значение к светодиоду без публикации (чтобы не было цикла).
    void applyRetainedState(const String& payload);
};

#endif