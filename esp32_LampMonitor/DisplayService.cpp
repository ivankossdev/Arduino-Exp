// ============================================================
// DisplayService — реализация
// ============================================================
// ИЗМЕНЕНО: удалены updateButton(), updateResetButton(),
//           performFactoryReset() и всё, что связано с GPIO.
//           Логика кнопок теперь в ButtonService, а фабричный
//           сброс — в AppState::performFactoryReset().
// ИЗМЕНЕНО: добавлены публичные методы-обработчики событий от
//           ButtonService: onServiceButtonPressed, onResetHoldStart,
//           onResetHoldTick, onResetHoldCancel, showResettingScreen.
// ============================================================

#include "DisplayService.h"
#include "log.h"

DisplayService::DisplayService(StateManager& stateManager,
                               WiFiService& wifiService,
                               MqttService& mqttService,
                               LampStateService& lampStateService)
    : _stateManager(stateManager),
      _wifiService(wifiService),
      _mqttService(mqttService),
      _lampStateService(lampStateService),
      _tft(),
      _lastUpdate(0),
      _updateInterval(DISPLAY_UPDATE_INTERVAL_MS),
      _view(DisplayView::LAMP),
      _serviceViewUntil(0),
      _lastWifi(false),
      _lastLampMqtt(false),
      _lastLampHasData(false),
      _resetUiActive(false),
      _currentState(AppStateEnum::IDLE),
      _fullRedraw(true)
{
    _stateManager.setOnStateChange([this](AppStateEnum newState) {
        this->handleStateChange(newState);
    });

    _lampStateService.setOnStateChange([this](const String& /*payload*/) {
        if (_resetUiActive) return;
        if (_view == DisplayView::LAMP) {
            this->drawLampView();
        }
    });
}

bool DisplayService::begin() {
    static bool inited = false;
    if (inited) return true;

    _tft.init();
    _tft.setRotation(1);
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("ESP32");
    _tft.println("Load...");

    String wifiQr = "WIFI:S:" AP_SSID ";T:WPA;P:" AP_PASSWORD ";;";
    qrcode_initText(&_qr, _qrData, AP_QR_VERSION, AP_QR_ECC, wifiQr.c_str());

    LOG_LN(LOG_CAT_TFT, "[TFT] OK");
    inited = true;
    return true;
}

void DisplayService::update() {
    // Во время удержания левой кнопки экран занят счётчиком —
    // ничего не трогаем, кадр рисует onResetHoldTick.
    if (_resetUiActive) return;

    unsigned long now = millis();

    // Возврат из SERVICE view по таймауту
    if (_view == DisplayView::SERVICE &&
        (long)(now - _serviceViewUntil) >= 0) {
        _view = DisplayView::LAMP;
        _fullRedraw = true;
        drawScreen();
    }

    if (now - _lastUpdate < _updateInterval) return;
    _lastUpdate = now;

    if (_currentState != AppStateEnum::IDLE &&
        _currentState != AppStateEnum::CONNECTED) {
        return;
    }

    if (_view == DisplayView::SERVICE) {
        drawServiceView();
        return;
    }

    bool wifi = _wifiService.isConnected();
    if (wifi != _lastWifi) {
        _lastWifi = wifi;
        _fullRedraw = true;
    }

    if (!wifi) {
        if (_fullRedraw) {
            drawApMode();
            _fullRedraw = false;
        }
        return;
    }

    bool   mqtt    = _mqttService.isConnected();
    bool   hasData = _lampStateService.hasData();
    String payload = hasData ? _lampStateService.getPayload() : String("");

    if (_fullRedraw ||
        mqtt    != _lastLampMqtt ||
        hasData != _lastLampHasData ||
        payload != _lastLampPayload) {
        _lastLampMqtt    = mqtt;
        _lastLampHasData = hasData;
        _lastLampPayload = payload;
        drawLampView();
    }
    _fullRedraw = false;
}

// ============================================================
// Обработчики событий от ButtonService
// ============================================================

void DisplayService::onServiceButtonPressed() {
    if (_currentState != AppStateEnum::IDLE &&
        _currentState != AppStateEnum::CONNECTED) {
        return;
    }
    if (!_wifiService.isConnected()) return;
    if (_view == DisplayView::SERVICE) return;

    _view = DisplayView::SERVICE;
    _serviceViewUntil = millis() + SERVICE_VIEW_TIMEOUT_MS;
    LOG_LN(LOG_CAT_TFT, "[TFT] view: SERVICE");
    drawServiceView();
}

void DisplayService::onResetHoldStart() {
    _resetUiActive = true;
    drawResetHoldScreen(BUTTON_RESET_HOLD_MS / 1000);
}

void DisplayService::onResetHoldTick(int secLeft) {
    drawResetHoldScreen(secLeft);
}

void DisplayService::onResetHoldCancel() {
    _resetUiActive = false;
    _fullRedraw = true;
    drawScreen();
}

void DisplayService::showResettingScreen() {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_RED, TFT_BLACK);
    _tft.setTextSize(3);
    _tft.setCursor(0, 40);
    _tft.println("RESETTING");
    _tft.setTextSize(1);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setCursor(0, 90);
    _tft.println("Erasing settings...");
    _tft.setCursor(0, 100);
    _tft.println("Rebooting...");
}

// ============================================================
// Отрисовка экрана удержания
// ============================================================
void DisplayService::drawResetHoldScreen(int secondsLeft) {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_RED, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 15);
    _tft.println("FACTORY RESET");

    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(3);
    _tft.setCursor(0, 50);
    _tft.print("Hold: ");
    _tft.print(secondsLeft);
    _tft.print("s");

    _tft.setTextSize(1);
    _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    _tft.setCursor(0, 110);
    _tft.println("Release to cancel");
}

// ============================================================
// Смена состояния приложения
// ============================================================
void DisplayService::handleStateChange(AppStateEnum newState) {
    _currentState = newState;
    _fullRedraw = true;
    _view = DisplayView::LAMP;
    _serviceViewUntil = 0;

    if (!_resetUiActive) {
        drawScreen();
    }
}

void DisplayService::drawScreen() {
    switch (_currentState) {
        case AppStateEnum::IDLE:
        case AppStateEnum::CONNECTED:
            if (!_wifiService.isConnected()) {
                drawApMode();
            } else if (_view == DisplayView::SERVICE) {
                drawServiceView();
            } else {
                drawLampView();
            }
            break;
        case AppStateEnum::SCANNING:
            drawScanning();
            break;
        case AppStateEnum::CONNECTING:
            drawConnecting();
            break;
        case AppStateEnum::ERROR:
            drawError();
            break;
        default:
            drawApMode();
            break;
    }
    _fullRedraw = false;
}

// ============================================================
// AP-режим с QR-кодом
// ============================================================
void DisplayService::drawApMode() {
    _tft.fillScreen(TFT_BLACK);

    _tft.setTextColor(TFT_CYAN, TFT_BLACK);
    _tft.setTextSize(1);
    _tft.setCursor(0, 5);
    _tft.println("=== AP MODE ===");

    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setCursor(0, 25);
    _tft.print("SSID:");
    _tft.setCursor(0, 38);
    _tft.println(AP_SSID);

    _tft.setCursor(0, 56);
    _tft.print("Pass:");
    _tft.setCursor(0, 69);
    _tft.println(AP_PASSWORD);

    _tft.setCursor(0, 87);
    _tft.print("IP: 192.168.4.1");

    _tft.setTextColor(TFT_GREEN, TFT_BLACK);
    _tft.setCursor(0, 110);
    _tft.println("Scan QR ->");

    drawQrCode();
}

void DisplayService::drawQrCode() {
    const int scale = AP_QR_SCALE;
    const int size  = _qr.size;
    const int px    = size * scale;

    int x0 = SCREEN_WIDTH  - px - AP_QR_MARGIN;
    int y0 = (SCREEN_HEIGHT - px) / 2;

    _tft.fillRect(x0, y0, px, px, TFT_WHITE);

    for (uint8_t y = 0; y < size; y++) {
        for (uint8_t x = 0; x < size; x++) {
            if (qrcode_getModule(&_qr, x, y)) {
                _tft.fillRect(x0 + x * scale,
                              y0 + y * scale,
                              scale, scale,
                              TFT_BLACK);
            }
        }
    }
}

// ============================================================
// LAMP view
// ============================================================
void DisplayService::drawLampView() {
    bool   mqtt    = _mqttService.isConnected();
    bool   hasData = _lampStateService.hasData();
    String payload = hasData ? _lampStateService.getPayload() : String("");

    _tft.fillScreen(TFT_BLACK);

    if (!mqtt) {
        drawLampUnknown();
        return;
    }

    if (!hasData) {
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(8);
        _tft.setCursor((SCREEN_WIDTH - 48) / 2, (SCREEN_HEIGHT - 64) / 2);
        _tft.print("?");
        return;
    }

    bool isOn = (payload == "ON");
    drawLampIcon(isOn);
}

void DisplayService::drawLampIcon(bool isOn) {
    const int cx = SCREEN_WIDTH  / 2;
    const int cy = SCREEN_HEIGHT / 2;
    const int r  = 40;

    uint16_t color = isOn ? TFT_YELLOW : TFT_DARKGREY;

    _tft.fillCircle(cx, cy, r, color);

    if (!isOn) return;

    const int rIn  = 48;
    const int rOut = 62;
    const float diagScale = 0.7071f;

    _tft.drawWideLine(cx + rIn,  cy, cx + rOut, cy, 5, color);
    _tft.drawWideLine(cx - rIn,  cy, cx - rOut, cy, 5, color);
    _tft.drawWideLine(cx, cy - rIn, cx, cy - rOut, 5, color);
    _tft.drawWideLine(cx, cy + rIn, cx, cy + rOut, 5, color);

    int inX  = (int)(rIn  * diagScale);
    int inY  = (int)(rIn  * diagScale);
    int outX = (int)(rOut * diagScale);
    int outY = (int)(rOut * diagScale);

    _tft.drawWideLine(cx + inX, cy - inY, cx + outX, cy - outY, 5, color);
    _tft.drawWideLine(cx + inX, cy + inY, cx + outX, cy + outY, 5, color);
    _tft.drawWideLine(cx - inX, cy - inY, cx - outX, cy - outY, 5, color);
    _tft.drawWideLine(cx - inX, cy + inY, cx - outX, cy + outY, 5, color);
}

void DisplayService::drawLampUnknown() {
    _tft.fillScreen(TFT_DARKGREY);
    _tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    _tft.setTextSize(8);
    _tft.setCursor((SCREEN_WIDTH - 96) / 2, (SCREEN_HEIGHT - 64) / 2);
    _tft.print("--");
}

// ============================================================
// SERVICE view
// ============================================================
void DisplayService::drawServiceView() {
    _tft.fillScreen(TFT_BLACK);

    _tft.setTextColor(TFT_CYAN, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("=== SERVICE ===");

    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setCursor(0, 30);
    if (_wifiService.isConnected()) {
        _tft.print("IP: ");
        _tft.println(_wifiService.getIP().toString());
    } else {
        _tft.println("IP: --");
    }

    _tft.setCursor(0, 58);
    if (_mqttService.isConnected()) {
        _tft.setTextColor(TFT_GREEN, TFT_BLACK);
        _tft.println("MQTT: OK");
    } else {
        _tft.setTextColor(TFT_RED, TFT_BLACK);
        _tft.println("MQTT: LOST");
    }

    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setCursor(0, 88);
    int rssi = WiFi.RSSI();
    _tft.print("RSSI: ");
    _tft.print(rssi);
    _tft.print(" dBm");

    int bars = 0;
    if (rssi > -55)      bars = 4;
    else if (rssi > -65) bars = 3;
    else if (rssi > -75) bars = 2;
    else if (rssi > -85) bars = 1;

    const int bx  = 200;
    const int by  = 92;
    const int bw  = 6;
    const int gap = 3;
    for (int i = 0; i < 4; i++) {
        int h = 8 + i * 6;
        int x = bx + i * (bw + gap);
        int y = by - h;
        if (i < bars) {
            _tft.fillRect(x, y, bw, h, TFT_GREEN);
        } else {
            _tft.drawRect(x, y, bw, h, TFT_DARKGREY);
        }
    }
}

// ============================================================
// Служебные экраны
// ============================================================
void DisplayService::drawScanning() {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_CYAN, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("=== SCANNING ===");
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(1);
    _tft.println("Searching...");
    _tft.println("Please wait");
}

void DisplayService::drawConnecting() {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("=== CONNECTING ===");
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(1);
    _tft.println("Connecting to");
    _tft.println("Wi-Fi...");
}

void DisplayService::drawError() {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_RED, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("!!! ERROR !!!");
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(1);
    _tft.println("Failed to");
    _tft.println("connect to");
    _tft.println("Wi-Fi");
}