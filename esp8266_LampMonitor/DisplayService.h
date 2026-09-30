#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"
#include "LampStateService.h"   // === ИЗМЕНЕНО: вместо forward-декларации AppState ===

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

class DisplayService {
public:
    // === ИЗМЕНЕНО: больше не зависит от AppState (убрана циклическая зависимость).
    // Вместо этого — прямая ссылка на LampStateService для чтения состояния.
    DisplayService(StateManager& stateManager,
                   WiFiService& wifiService,
                   MqttService& mqttService,
                   LampStateService& lampStateService);

    bool begin();
    void update();

private:
    StateManager& _stateManager;
    WiFiService& _wifiService;
    MqttService& _mqttService;
    LampStateService& _lampStateService;   // === ИЗМЕНЕНО ===
    Adafruit_SSD1306 _display;

    unsigned long _lastUpdate;
    unsigned long _updateInterval;

    void drawScreen();
    void drawApMode();
    void drawClientMode();
    void drawError();
    void drawConnecting();
    void drawScanning();

    void handleStateChange(AppStateEnum newState);

    AppStateEnum _currentState;
};

#endif