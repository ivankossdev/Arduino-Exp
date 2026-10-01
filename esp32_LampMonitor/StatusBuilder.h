#ifndef STATUS_BUILDER_H
#define STATUS_BUILDER_H

#include <Arduino.h>
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"
#include "LampStateService.h"   // === ИЗМЕНЕНО: вместо LedService ===

class StatusBuilder {
public:
    StatusBuilder(StateManager& stateManager,
                  WiFiService& wifiService,
                  MqttService& mqttService,
                  LampStateService& lampStateService);  // === ИЗМЕНЕНО ===

    String buildJson();
    String buildText() const;

private:
    StateManager& _stateManager;
    WiFiService& _wifiService;
    MqttService& _mqttService;
    LampStateService& _lampStateService;   // === ИЗМЕНЕНО ===
};

#endif