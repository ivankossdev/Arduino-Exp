// ============================================================
// DisplayService — управление дисплеем ST7789 (TTGO T-Display)
// ============================================================
// ИЗМЕНЕНО: буфер QR-кода объявлен через AP_QR_BUFFER_SIZE
//           из config.h, а не через qrcode_getBufferSize() —
//           она не является constexpr и не годится для размера массива.
// ============================================================

#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include "qrcode.h"             // локальная копия ricmoo в корне проекта

#include "config.h"
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

    QRCode  _qr;
    // ИЗМЕНЕНО: было qrcode_getBufferSize(AP_QR_VERSION) —
    // функция не constexpr, компилятор C++ её не принимает.
    uint8_t _qrData[AP_QR_BUFFER_SIZE];

    unsigned long _lastUpdate;
    unsigned long _updateInterval;

    void drawScreen();
    void drawApMode();
    void drawClientMode();
    void drawError();
    void drawConnecting();
    void drawScanning();

    void drawQrCode();

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