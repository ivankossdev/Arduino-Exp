#ifndef STATUS_BUILDER_H
#define STATUS_BUILDER_H

#include <Arduino.h>
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"
#include "LedService.h"

// === НОВЫЙ КЛАСС (шаг 1 рефакторинга AppState) ===
// Сборка статуса устройства: в JSON и в текстовом виде.
// Раньше это жило в AppState::getStatusJson и AppState::getStatusString.
// JSON собирается через ArduinoJson — меньше риск сломать его экранированием,
// и проще расширять новыми полями (uptime, ошибки и т.п.).
class StatusBuilder {
public:
    StatusBuilder(StateManager& stateManager,
                  WiFiService& wifiService,
                  MqttService& mqttService,
                  LedService& ledService);

    // JSON для /status. Собирается через ArduinoJson.
    String buildJson();

    // Короткая текстовая строка статуса Wi-Fi.
    String buildText() const;

private:
    StateManager& _stateManager;
    WiFiService& _wifiService;
    MqttService& _mqttService;
    LedService& _ledService;
};

#endif