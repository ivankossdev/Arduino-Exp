// ============================================================
// DisplayService — реализация для TFT_eSPI (ST7789V)
// ============================================================
// ИЗМЕНЕНО: добавлен сброс по удержанию левой кнопки (GPIO0).
//           Пока идёт удержание — экран занят счётчиком; любые
//           другие обновления пропускаются.
//           Смена состояния приложения и приход новых payload'ов
//           во время удержания игнорируются на экране, но данные
//           обновляются в сервисах как обычно.
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
      _lastButtonState(false),
      _lastButtonChange(0),
      _lastWifi(false),
      _lastLampMqtt(false),
      _lastLampHasData(false),
      _resetHoldActive(false),
      _resetHoldStart(0),
      _lastResetSecondShown(-1),
      _resetButtonRaw(false),
      _resetButtonChange(0),
      _currentState(AppStateEnum::IDLE),
      _fullRedraw(true)
{
    _stateManager.setOnStateChange([this](AppStateEnum newState) {
        this->handleStateChange(newState);
    });

    _lampStateService.setOnStateChange([this](const String& /*payload*/) {
        // Во время удержания левой кнопки экран занят — не трогаем
        if (_resetHoldActive) return;
        if (_view == DisplayView::LAMP) {
            this->drawLampView();
        }
    });
}

bool DisplayService::begin() {
    static bool inited = false;
    if (inited) return true;

    // === Правая кнопка (GPIO35) — сервисный экран ===
    pinMode(BUTTON_SERVICE_PIN, INPUT);
    _lastButtonState = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
    _lastButtonChange = millis();

    // === НОВОЕ: левая кнопка (GPIO0) — сброс.
    // Внутренний pull-up, нажатие = LOW.
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    _resetButtonRaw = (digitalRead(BUTTON_RESET_PIN) == LOW);
    _resetButtonChange = millis();

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
    updateButton();

    // Пока идёт удержание левой кнопки — никаких других обновлений экрана.
    if (_resetHoldActive) return;

    unsigned long now = millis();

    // Возврат из SERVICE view по таймауту
    if (_view == DisplayView::SERVICE &&
        (long)(now - _serviceViewUntil) >= 0) {
        _view = DisplayView::LAMP;
        _lastButtonState = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
        _lastButtonChange = now;
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

    // === НОВОЕ: отслеживаем смену Wi-Fi (AP ↔ клиент).
    // Без этого при старте в AP-режиме первый update() рисовал
    // Lamp view с "--", а QR появлялся только после нажатия кнопки.
    bool wifi = _wifiService.isConnected();
    if (wifi != _lastWifi) {
        _lastWifi = wifi;
        _fullRedraw = true;
    }

    // Wi-Fi не подключён — значит, мы в AP-режиме. Рисуем QR.
    // AP статичен, так что перерисовываем только при смене состояния.
    if (!wifi) {
        if (_fullRedraw) {
            drawApMode();
            _fullRedraw = false;
        }
        return;
    }

    // Wi-Fi подключён — Lamp view с построчным кэшем.
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
// Кнопки: сначала левая (сброс), затем правая (сервис)
// ============================================================
void DisplayService::updateButton() {
    unsigned long now = millis();

    // Левая кнопка — сброс. Активна в любом состоянии.
    updateResetButton(now);

    // Если идёт удержание — правая кнопка игнорируется.
    if (_resetHoldActive) return;

    // --- Правая кнопка: логика как была ---
    if (_currentState != AppStateEnum::IDLE &&
        _currentState != AppStateEnum::CONNECTED) {
        _lastButtonState = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
        return;
    }
    if (!_wifiService.isConnected()) {
        _lastButtonState = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
        return;
    }

    bool raw = (digitalRead(BUTTON_SERVICE_PIN) == LOW);

    if (raw != _lastButtonState) {
        _lastButtonChange = now;
        _lastButtonState = raw;
        return;
    }

    if (_view == DisplayView::SERVICE) return;

    if (raw && (now - _lastButtonChange) >= BUTTON_DEBOUNCE_MS) {
        _view = DisplayView::SERVICE;
        _serviceViewUntil = now + SERVICE_VIEW_TIMEOUT_MS;
        LOG_LN(LOG_CAT_TFT, "[TFT] view: SERVICE");
        drawServiceView();
    }
}

// ============================================================
// Левая кнопка: конечный автомат удержания для сброса
// ============================================================
// Логика:
//   1. Ждём стабильного нажатия после антидребезга.
//   2. Стартуем hold; каждую секунду обновляем счётчик на экране.
//   3. Если отпустили до BUTTON_RESET_HOLD_MS — отмена, redraw.
//   4. Если додержали — performFactoryReset().
void DisplayService::updateResetButton(unsigned long now) {
    bool raw = (digitalRead(BUTTON_RESET_PIN) == LOW);

    // Антидребезг: фиксируем изменение состояния
    if (raw != _resetButtonRaw) {
        _resetButtonChange = now;
        _resetButtonRaw = raw;
    }

    if (!_resetHoldActive) {
        // Проверяем, не пора ли начать удержание
        if (raw && (now - _resetButtonChange) >= BUTTON_DEBOUNCE_MS) {
            _resetHoldActive = true;
            _resetHoldStart = now;
            _lastResetSecondShown = -1;
            drawResetHoldScreen(BUTTON_RESET_HOLD_MS / 1000);
        }
        return;
    }

    // Hold активен. Проверяем отпускание.
    if (!raw && (now - _resetButtonChange) >= BUTTON_DEBOUNCE_MS) {
        // Отпустили до завершения — отмена
        _resetHoldActive = false;
        _fullRedraw = true;
        drawScreen();
        return;
    }

    // Проверяем, не достигли ли порога
    unsigned long heldMs = now - _resetHoldStart;
    if (heldMs >= BUTTON_RESET_HOLD_MS) {
        performFactoryReset();
        return;
    }

    // Обновляем счётчик раз в секунду
    int secLeft = (BUTTON_RESET_HOLD_MS - heldMs + 999) / 1000;
    if (secLeft != _lastResetSecondShown) {
        _lastResetSecondShown = secLeft;
        drawResetHoldScreen(secLeft);
    }
}

// ============================================================
// Экран удержания левой кнопки
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
// Сброс настроек и перезагрузка
// ============================================================
// Порядок:
//   1. Показать "RESETTING" (визуальная отдача).
//   2. Стереть Wi-Fi credentials (в цикле по одной).
//   3. Стереть MQTT-настройки (сброс на значения по умолчанию).
//   4. Дождаться отпускания левой кнопки — иначе после рестарта
//      плата уйдёт в download mode (GPIO0 strapping).
//   5. ESP.restart().
void DisplayService::performFactoryReset() {
    LOG_LN(LOG_CAT_SYS, "[SYS] Factory reset triggered");

    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_RED, TFT_BLACK);
    _tft.setTextSize(3);
    _tft.setCursor(0, 40);
    _tft.println("RESETTING");
    _tft.setTextSize(1);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setCursor(0, 90);
    _tft.println("Erasing settings...");

    // Стираем Wi-Fi credentials по одной
    while (_wifiService.getSavedCount() > 0) {
        _wifiService.deleteSavedNetwork(0);
    }

    // Сбрасываем MQTT-настройки на дефолтные
    _mqttService.configure("", MQTT_DEFAULT_PORT, "", "", MQTT_DEFAULT_STATE_TOPIC);

    // Ждём отпускания левой кнопки, чтобы при рестарте не уйти
    // в download mode. Таймаут на всякий случай.
    unsigned long waitStart = millis();
    while (digitalRead(BUTTON_RESET_PIN) == LOW &&
           millis() - waitStart < BUTTON_RESET_RELEASE_WAIT_MS) {
        delay(50);
    }

    _tft.setCursor(0, 100);
    _tft.println("Rebooting...");

    delay(300);
    ESP.restart();
}

void DisplayService::handleStateChange(AppStateEnum newState) {
    _currentState = newState;
    _fullRedraw = true;
    _view = DisplayView::LAMP;
    _serviceViewUntil = 0;

    // Во время удержания левой кнопки экран занят счётчиком.
    // Смена состояния будет отрисована при отмене или применена
    // после перезагрузки.
    if (!_resetHoldActive) {
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
// Далее — без изменений: drawApMode, drawQrCode, drawLampView,
// drawLampIcon, drawLampUnknown, drawServiceView, drawScanning,
// drawConnecting, drawError. Копируются из предыдущей версии.
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