// ============================================================
// DisplayService — управление дисплеем ST7789 (TTGO T-Display)
// ============================================================
// ИЗМЕНЕНО: логика кнопок вынесена в ButtonService.
//           DisplayService теперь реагирует на события через
//           публичные методы onServiceButtonPressed(), onResetHoldStart()
//           и т. п., которые вызывает AppState по колбэкам от ButtonService.
// ИЗМЕНЕНО: удалены поля состояния кнопок и методы updateButton(),
//           updateResetButton(), performFactoryReset().
// ИЗМЕНЕНО: добавлен флаг _resetUiActive — блокирует обновления экрана
//           во время удержания левой кнопки.
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

    // === НОВОЕ: события от ButtonService ===
    void onServiceButtonPressed();
    void onResetHoldStart();
    void onResetHoldTick(int secLeft);
    void onResetHoldCancel();
    void showResettingScreen();

private:
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

    DisplayView   _view;
    unsigned long _serviceViewUntil;

    bool   _lastWifi;
    bool   _lastLampMqtt;
    bool   _lastLampHasData;
    String _lastLampPayload;

    // === НОВОЕ: идёт отображение удержания сброса ===
    // Блокирует обычные обновления экрана, пока пользователь
    // держит левую кнопку. Устанавливается через onResetHoldStart(),
    // сбрасывается в onResetHoldCancel().
    bool _resetUiActive;

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

    void drawResetHoldScreen(int secondsLeft);

    void handleStateChange(AppStateEnum newState);

    AppStateEnum _currentState;

    bool _fullRedraw;
};

#endif