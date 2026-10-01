// ============================================================
// DisplayService — управление дисплеем ST7789 (TTGO T-Display)
// ============================================================
// ИЗМЕНЕНО: Adafruit_SSD1306 → TFT_eSPI (цветной ST7789V)
// ИЗМЕНЕНО: разрешение 128x64 → 240x135 (альбомная ориентация)
// ИЗМЕНЕНО: WebServer.h подключается ДО TFT_eSPI.h — иначе конфликт
//           макроса FS_NO_GLOBALS (TFT_eSPI с SMOOTH_FONT глушит
//           глобальный тип FS, и WebServer.h не может его найти)
// ============================================================

#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>

// ВАЖНО: этот include должен идти ПЕРЕД TFT_eSPI.h
// WebServer.h использует глобальный тип FS из ESP32 Core,
// который пропадает, если TFT_eSPI успеет определить FS_NO_GLOBALS.
#include <WebServer.h>

#include <TFT_eSPI.h>
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"
#include "LampStateService.h"

// ИЗМЕНЕНО: разрешение дисплея TTGO T-Display — 135x240
// (в альбомной ориентации: 240x135)
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 135

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

    TFT_eSPI _tft;             // ИЗМЕНЕНО: было Adafruit_SSD1306 _display

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