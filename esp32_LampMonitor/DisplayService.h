// ============================================================
// DisplayService — управление дисплеем ST7789 (TTGO T-Display)
// ============================================================
// ИЗМЕНЕНО: Adafruit_SSD1306 → TFT_eSPI (цветной ST7789V)
// ИЗМЕНЕНО: разрешение 128x64 → 240x135 (альбомная ориентация)
// ИЗМЕНЕНО: WebServer.h подключается ДО TFT_eSPI.h — иначе конфликт
//           макроса FS_NO_GLOBALS
// ИЗМЕНЕНО: добавлен кэш последних отрисованных значений —
//           устраняет мерцание от периодических полных перерисовок
// ============================================================

#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>

// ВАЖНО: этот include должен идти ПЕРЕД TFT_eSPI.h
#include <WebServer.h>
#include <TFT_eSPI.h>

#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"
#include "LampStateService.h"

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

    // === НОВОЕ: кэш последних отрисованных значений ===
    // Экран полностью перерисовывается только при смене состояния
    // приложения. Периодические тики обновляют только изменившиеся блоки.
    bool   _fullRedraw;     // принудительная полная перерисовка (после смены состояния)
    bool   _lastWifi;
    String _lastSSID;
    String _lastIP;
    int    _lastRSSI;
    bool   _lastMqtt;
    bool   _lastHasLamp;
    String _lastLamp;
};

#endif