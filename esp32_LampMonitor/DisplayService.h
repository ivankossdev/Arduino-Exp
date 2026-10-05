// ============================================================
// DisplayService — управление дисплеем ST7789 (TTGO T-Display)
// ============================================================
// ИЗМЕНЕНО: добавлены два режима отображения — LAMP (основной)
//           и SERVICE (по кнопке, на 20 секунд).
// ИЗМЕНЕНО: drawClientMode() удалён — вместо него drawLampView()
//           и drawServiceView().
// ИЗМЕНЕНО: добавлены поля для кнопки (антидребезг) и кэш
//           состояния Lamp view, чтобы не перерисовывать зря.
// ============================================================

#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include "qrcode.h"

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
    // === НОВОЕ: режим отображения ===
    // LAMP     — основной, крупная иконка лампы
    // SERVICE  — служебный, IP / MQTT / RSSI, активируется кнопкой
    enum class DisplayView {
        LAMP,
        SERVICE
    };

    StateManager& _stateManager;
    WiFiService& _wifiService;
    MqttService& _mqttService;
    LampStateService& _lampStateService;

    TFT_eSPI _tft;

    QRCode  _qr;
    uint8_t _qrData[AP_QR_BUFFER_SIZE];

    unsigned long _lastUpdate;
    unsigned long _updateInterval;

    // === НОВОЕ: состояние режимов и кнопки ===
    DisplayView   _view;
    unsigned long _serviceViewUntil;      // millis() до которого держать SERVICE
    bool          _lastButtonState;       // для антидребезга
    unsigned long _lastButtonChange;      // момент последнего изменения кнопки

    // === НОВОЕ: кэш Lamp view, чтобы не мигало ===
    bool   _lastLampMqtt;
    bool   _lastLampHasData;
    String _lastLampPayload;

    void drawScreen();
    void drawApMode();
    void drawLampView();
    void drawServiceView();
    void drawLampIcon(bool isOn);
    void drawLampUnknown();
    void drawError();
    void drawConnecting();
    void drawScanning();
    void drawQrCode();

    void updateButton();

    void handleStateChange(AppStateEnum newState);

    AppStateEnum _currentState;

    bool _fullRedraw;
};

#endif