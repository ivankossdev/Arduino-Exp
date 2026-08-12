#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

class DisplayService {
public:
    DisplayService(StateManager& stateManager, WiFiService& wifiService, MqttService& mqttService);

    bool begin();
    void update();

private:
    StateManager& _stateManager;
    WiFiService& _wifiService;
    MqttService& _mqttService;
    Adafruit_SSD1306 _display;

    unsigned long _lastUpdate;
    unsigned long _updateInterval;

    void drawScreen();
    void drawApMode();
    void drawClientMode();
    void drawError();
    void drawConnecting();
    void drawScanning();   // <--- добавлен прототип

    void setState(AppStateEnum newState);
    void handleStateChange(AppStateEnum newState);

    AppStateEnum _currentState;
};

#endif