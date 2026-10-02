// ============================================================
// DisplayService — управление дисплеем ST7789 (TTGO T-Display)
// ============================================================
// ИЗМЕНЕНО: SCREEN_WIDTH/SCREEN_HEIGHT перенесены в config.h
// ИЗМЕНЕНО: добавлен #include "config.h"
// ============================================================

#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>
#include <WebServer.h>
#include <TFT_eSPI.h>

#include "config.h"             // ИЗМЕНЕНО: константы вынесены сюда
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"
#include "LampStateService.h"

class DisplayService {
public:
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
    LampStateService& _lampStateService;

    TFT_eSPI _tft;

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

    bool   _fullRedraw;
    bool   _lastWifi;
    String _lastSSID;
    String _lastIP;
    int    _lastRSSI;
    bool   _lastMqtt;
    bool   _lastHasLamp;
    String _lastLamp;
};

#endif