// ============================================================
// DisplayService — реализация для TFT_eSPI (ST7789V)
// ============================================================
// ИЗМЕНЕНО: вместо drawClientMode() два новых вида —
//   drawLampView()     — крупная иконка ON/OFF, либо "--", либо "?"
//   drawServiceView()  — IP / MQTT / RSSI с палочками
// ИЗМЕНЕНО: добавлена работа с правой кнопкой (GPIO35):
//   одиночное нажатие → SERVICE view на SERVICE_VIEW_TIMEOUT_MS
//   повторное нажатие в SERVICE view игнорируется
//   по таймауту — возврат в LAMP view
// ИЗМЕНЕНО: кэш Lamp view (_lastLampMqtt / _lastLampHasData /
//   _lastLampPayload), чтобы не перерисовывать иконку зря.
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
      _lastLampMqtt(false),
      _lastLampHasData(false),
      _currentState(AppStateEnum::IDLE),
      _fullRedraw(true)
{
    _stateManager.setOnStateChange([this](AppStateEnum newState) {
        this->handleStateChange(newState);
    });

    // === ИЗМЕНЕНО: колбэк перерисовывает только если сейчас видна иконка лампы.
    // Если активен SERVICE view — не трогаем его, лампа обновится при возврате.
    _lampStateService.setOnStateChange([this](const String& /*payload*/) {
        if (_view == DisplayView::LAMP) {
            this->drawLampView();
        }
    });
}

bool DisplayService::begin() {
    static bool inited = false;
    if (inited) return true;

    // Инициализация правой кнопки. Внешний pull-up на плате,
    // поэтому INPUT без INPUT_PULLUP.
    pinMode(BUTTON_SERVICE_PIN, INPUT);
    _lastButtonState = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
    _lastButtonChange = millis();

    _tft.init();
    _tft.setRotation(1);
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("ESP32");
    _tft.println("Load...");

    // Подготовка QR-кода для AP-режима
    String wifiQr = "WIFI:S:" AP_SSID ";T:WPA;P:" AP_PASSWORD ";;";
    qrcode_initText(&_qr, _qrData, AP_QR_VERSION, AP_QR_ECC, wifiQr.c_str());

    LOG_LN(LOG_CAT_TFT, "[TFT] OK");
    inited = true;
    return true;
}

void DisplayService::update() {
    // === НОВОЕ: кнопка опрашивается при каждом вызове update(),
    // не по таймеру. Это даёт отзывчивость в пределах миллисекунд.
    updateButton();

    unsigned long now = millis();

    // === НОВОЕ: проверка таймаута SERVICE view ===
    // Используем (long) для корректной работы при переполнении millis().
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

    // Периодические обновления только в рабочих состояниях.
    if (_currentState != AppStateEnum::IDLE &&
        _currentState != AppStateEnum::CONNECTED) {
        return;
    }

    // В SERVICE view обновляем экран каждые 2 секунды — так RSSI остаётся свежим.
    if (_view == DisplayView::SERVICE) {
        drawServiceView();
        return;
    }

    // === LAMP view: перерисовка только при смене данных ===
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
// Обработка кнопки
// ============================================================
// Активна только в рабочем клиентском режиме. В AP/SCANNING/
// CONNECTING/ERROR — игнорируется. Повторное нажатие во время
// показа SERVICE view — игнорируется.
void DisplayService::updateButton() {
    // Не рабочий режим — кнопка неактивна
    if (_currentState != AppStateEnum::IDLE &&
        _currentState != AppStateEnum::CONNECTED) {
        _lastButtonState = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
        return;
    }
    // AP-режим — кнопка неактивна
    if (!_wifiService.isConnected()) {
        _lastButtonState = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
        return;
    }

    bool raw = (digitalRead(BUTTON_SERVICE_PIN) == LOW);
    unsigned long now = millis();

    // Фиксируем изменение состояния кнопки для антидребезга.
    if (raw != _lastButtonState) {
        _lastButtonChange = now;
        _lastButtonState = raw;
        return;
    }

    // Уже показываем SERVICE view — новые нажатия игнорируем.
    if (_view == DisplayView::SERVICE) {
        return;
    }

    // Стабильное нажатие после антидребезга — переключаемся в SERVICE.
    if (raw && (now - _lastButtonChange) >= BUTTON_DEBOUNCE_MS) {
        _view = DisplayView::SERVICE;
        _serviceViewUntil = now + SERVICE_VIEW_TIMEOUT_MS;
        LOG_LN(LOG_CAT_TFT, "[TFT] view: SERVICE");
        drawServiceView();
    }
}

// ============================================================
// Смена состояния приложения
// ============================================================
// При любой смене состояния возвращаемся в LAMP view и сбрасываем
// таймер сервисного экрана. Так после сбоя/восстановления Wi-Fi
// всегда попадаем на основной экран.
void DisplayService::handleStateChange(AppStateEnum newState) {
    _currentState = newState;
    _fullRedraw = true;
    _view = DisplayView::LAMP;
    _serviceViewUntil = 0;
    drawScreen();
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
// AP-режим: слева текст, справа QR-код
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
// LAMP view — основной экран
// ============================================================
// Показывает крупную иконку лампы:
//   MQTT OK + есть данные + payload == "ON"  → жёлтая иконка с лучами
//   MQTT OK + есть данные + payload != "ON"  → серая иконка без лучей
//   MQTT OK + данных нет                     → крупный "?"
//   MQTT потерян                             → серый фон + "--"
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
        // MQTT есть, но данных ещё не было
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(8);
        _tft.setCursor((SCREEN_WIDTH - 48) / 2, (SCREEN_HEIGHT - 64) / 2);
        _tft.print("?");
        return;
    }

    // Считаем лампу включённой, если payload == "ON".
    // Всё остальное — выключено (OFF, 0, что угодно).
    bool isOn = (payload == "ON");
    drawLampIcon(isOn);
}

// ============================================================
// Иконка лампы
// ============================================================
// ON  — жёлтый кружок + 8 лучей вокруг
// OFF — серый кружок без лучей
void DisplayService::drawLampIcon(bool isOn) {
    const int cx = SCREEN_WIDTH  / 2;    // 120
    const int cy = SCREEN_HEIGHT / 2;    // 67
    const int r  = 40;                    // радиус кружка

    uint16_t color = isOn ? TFT_YELLOW : TFT_DARKGREY;

    _tft.fillCircle(cx, cy, r, color);

    if (!isOn) {
        return;  // OFF — просто кружок
    }

    // ON — 8 лучей вокруг кружка.
    // Внутренний радиус 48, внешний 62; толщина линии 5.
    const int rIn  = 48;
    const int rOut = 62;
    const float diagScale = 0.7071f;   // sqrt(2)/2

    // 4 ортогональных луча
    _tft.drawWideLine(cx + rIn,  cy, cx + rOut, cy, 5, color);   // право
    _tft.drawWideLine(cx - rIn,  cy, cx - rOut, cy, 5, color);   // лево
    _tft.drawWideLine(cx, cy - rIn, cx, cy - rOut, 5, color);    // верх
    _tft.drawWideLine(cx, cy + rIn, cx, cy + rOut, 5, color);    // низ

    // 4 диагональных луча. Координаты умножаем на 0.707,
    // чтобы точки лежали на окружности радиусов rIn/rOut.
    int inX  = (int)(rIn  * diagScale);
    int inY  = (int)(rIn  * diagScale);
    int outX = (int)(rOut * diagScale);
    int outY = (int)(rOut * diagScale);

    _tft.drawWideLine(cx + inX, cy - inY, cx + outX, cy - outY, 5, color); // ↖
    _tft.drawWideLine(cx + inX, cy + inY, cx + outX, cy + outY, 5, color); // ↘
    _tft.drawWideLine(cx - inX, cy - inY, cx - outX, cy - outY, 5, color); // ↗
    _tft.drawWideLine(cx - inX, cy + inY, cx - outX, cy + outY, 5, color); // ↙
}

// ============================================================
// Иконка «нет связи с брокером»
// ============================================================
// Серый фон + крупное "--" по центру
void DisplayService::drawLampUnknown() {
    _tft.fillScreen(TFT_DARKGREY);
    _tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    _tft.setTextSize(8);
    // "--" = 2 символа × 48 px = 96 px; центрируем
    _tft.setCursor((SCREEN_WIDTH - 96) / 2, (SCREEN_HEIGHT - 64) / 2);
    _tft.print("--");
}

// ============================================================
// SERVICE view — служебный экран (по кнопке)
// ============================================================
// IP / MQTT: OK|LOST / RSSI: число + палочки
void DisplayService::drawServiceView() {
    _tft.fillScreen(TFT_BLACK);

    _tft.setTextColor(TFT_CYAN, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("=== SERVICE ===");

    // IP
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setCursor(0, 30);
    if (_wifiService.isConnected()) {
        _tft.print("IP: ");
        _tft.println(_wifiService.getIP().toString());
    } else {
        _tft.println("IP: --");
    }

    // MQTT
    _tft.setCursor(0, 58);
    if (_mqttService.isConnected()) {
        _tft.setTextColor(TFT_GREEN, TFT_BLACK);
        _tft.println("MQTT: OK");
    } else {
        _tft.setTextColor(TFT_RED, TFT_BLACK);
        _tft.println("MQTT: LOST");
    }

    // RSSI + палочки
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setCursor(0, 88);
    int rssi = WiFi.RSSI();
    _tft.print("RSSI: ");
    _tft.print(rssi);
    _tft.print(" dBm");

    // Палочки: 4 столбика, закрашено в зависимости от уровня.
    // Пороги: -55 / -65 / -75 / -85 dBm
    int bars = 0;
    if (rssi > -55)      bars = 4;
    else if (rssi > -65) bars = 3;
    else if (rssi > -75) bars = 2;
    else if (rssi > -85) bars = 1;

    const int bx   = 200;   // X начала палочек
    const int by   = 92;    // Y низа палочек
    const int bw   = 6;     // ширина палочки
    const int gap  = 3;     // зазор
    for (int i = 0; i < 4; i++) {
        int h = 8 + i * 6;              // высота: 8, 14, 20, 26
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
// Служебные экраны (без изменений)
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