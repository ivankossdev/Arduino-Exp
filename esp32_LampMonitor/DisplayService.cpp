// ============================================================
// DisplayService — реализация для TFT_eSPI (ST7789V)
// ============================================================
// ИЗМЕНЕНО: добавлен #include "log.h" — без него не работают
//           макросы LOG_LN/LOG в этом файле.
// ============================================================

#include "DisplayService.h"
#include "log.h"       // ИЗМЕНЕНО: макросы логирования

// Позиции строк в клиентском режиме (px, сверху вниз).
#define LINE_WIFI_Y   26
#define LINE_IP_Y     40
#define LINE_RSSI_Y   54
#define LINE_MQTT_Y   78
#define LINE_LAMP_Y   92
#define LINE_HEIGHT   14

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
      _currentState(AppStateEnum::IDLE),
      _fullRedraw(true),
      _lastWifi(false),
      _lastRSSI(-999),
      _lastMqtt(false),
      _lastHasLamp(false)
{
    _stateManager.setOnStateChange([this](AppStateEnum newState) {
        this->handleStateChange(newState);
    });

    _lampStateService.setOnStateChange([this](const String& /*payload*/) {
        this->drawScreen();
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

    // === НОВОЕ: подготовка QR-кода для AP-режима ===
    // Строка формата "WIFI:S:<SSID>;T:WPA;P:<пароль>;;" — её понимают
    // и Android, и iOS: после сканирования телефон предложит подключиться
    // к указанной сети с указанным паролем.
    //
    // Строка собирается на этапе компиляции через конкатенацию строковых
    // литералов AP_SSID и AP_PASSWORD из config.h.
    String wifiQr = "WIFI:S:" AP_SSID ";T:WPA;P:" AP_PASSWORD ";;";
    qrcode_initText(&_qr, _qrData, AP_QR_VERSION, AP_QR_ECC, wifiQr.c_str());

    LOG_LN(LOG_CAT_TFT, "[TFT] OK");
    inited = true;
    return true;
}

void DisplayService::update() {
    unsigned long now = millis();
    if (now - _lastUpdate < _updateInterval) return;
    _lastUpdate = now;

    if (!_fullRedraw) {
        if (_currentState != AppStateEnum::IDLE &&
            _currentState != AppStateEnum::CONNECTED) {
            return;
        }
    }

    drawScreen();
}

void DisplayService::handleStateChange(AppStateEnum newState) {
    _currentState = newState;
    _fullRedraw = true;
    drawScreen();
}

void DisplayService::drawScreen() {
    switch (_currentState) {
        case AppStateEnum::IDLE:
        case AppStateEnum::CONNECTED:
            if (_wifiService.isConnected()) {
                drawClientMode();
            } else {
                drawApMode();
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
// AP-режим: слева текст, справа QR-код для быстрого подключения
// ============================================================
void DisplayService::drawApMode() {
    _tft.fillScreen(TFT_BLACK);

    // --- Левая половина: заголовок и реквизиты точки доступа ---
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

    // --- Правая половина: QR-код ---
    drawQrCode();
}

// ============================================================
// Отрисовка QR-кода в правой части экрана
// ============================================================
// Алгоритм: рисуем белый квадрат-фон, затем поверх него рисуем
// чёрные модули через fillRect. Так как drawApMode() вызывается
// только при смене состояния, а не в цикле, скорость отрисовки
// некритична.
void DisplayService::drawQrCode() {
    const int scale = AP_QR_SCALE;
    const int size  = _qr.size;             // число модулей по стороне
    const int px    = size * scale;         // размер QR в пикселях

    // Позиционируем QR в правой части, по центру по вертикали
    int x0 = SCREEN_WIDTH  - px - AP_QR_MARGIN;
    int y0 = (SCREEN_HEIGHT - px) / 2;

    // Белая подложка
    _tft.fillRect(x0, y0, px, px, TFT_WHITE);

    // Чёрные модули
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

void DisplayService::drawClientMode() {
    bool   wifi    = _wifiService.isConnected();
    bool   mqtt    = _mqttService.isConnected();
    bool   hasLamp = _lampStateService.hasData();
    String ssid    = wifi ? _wifiService.getCurrentSSID() : String("");
    String ip      = wifi ? _wifiService.getIP().toString() : String("");
    int    rssi    = wifi ? (WiFi.RSSI() / RSSI_QUANTIZE_STEP) * RSSI_QUANTIZE_STEP : 0;
    String lamp    = hasLamp ? _lampStateService.getPayload() : String("");

    bool needFull = _fullRedraw || (_lastWifi != wifi);

    if (needFull) {
        _tft.fillScreen(TFT_BLACK);
        _tft.setTextColor(TFT_GREEN, TFT_BLACK);
        _tft.setTextSize(2);
        _tft.setCursor(0, 0);
        _tft.println("=== CLIENT ===");

        _lastSSID    = "";
        _lastIP      = "";
        _lastRSSI    = -999;
        _lastMqtt    = !mqtt;
        _lastHasLamp = !hasLamp;
        _lastLamp    = "";
    }

    if (needFull || _lastSSID != ssid) {
        _tft.fillRect(0, LINE_WIFI_Y, SCREEN_WIDTH, LINE_HEIGHT, TFT_BLACK);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(1);
        _tft.setCursor(0, LINE_WIFI_Y + 3);
        if (wifi) {
            _tft.print("WiFi: ");
            _tft.print(ssid);
        } else {
            _tft.print("WiFi: NO");
        }
        _lastSSID = ssid;
    }

    if (needFull || _lastIP != ip) {
        _tft.fillRect(0, LINE_IP_Y, SCREEN_WIDTH, LINE_HEIGHT, TFT_BLACK);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(1);
        _tft.setCursor(0, LINE_IP_Y + 3);
        if (wifi) {
            _tft.print("IP: ");
            _tft.print(ip);
        }
        _lastIP = ip;
    }

    if (needFull || _lastRSSI != rssi) {
        _tft.fillRect(0, LINE_RSSI_Y, SCREEN_WIDTH, LINE_HEIGHT, TFT_BLACK);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(1);
        _tft.setCursor(0, LINE_RSSI_Y + 3);
        if (wifi) {
            _tft.print("RSSI: ");
            _tft.print(rssi);
            _tft.print(" dBm");
        }
        _lastRSSI = rssi;
    }

    bool mqttChanged = (needFull || _lastMqtt != mqtt);

    if (mqttChanged) {
        _tft.fillRect(0, LINE_MQTT_Y, SCREEN_WIDTH, LINE_HEIGHT, TFT_BLACK);
        _tft.setTextSize(1);
        _tft.setCursor(0, LINE_MQTT_Y + 3);
        if (mqtt) {
            _tft.setTextColor(TFT_WHITE, TFT_BLACK);
            _tft.print("MQTT: OK");
        } else {
            _tft.setTextColor(TFT_RED, TFT_BLACK);
            _tft.print("MQTT: NO (LOST)");
        }
        _lastMqtt = mqtt;
    }

    if (needFull || mqttChanged || _lastHasLamp != hasLamp || _lastLamp != lamp) {
        _tft.fillRect(0, LINE_LAMP_Y, SCREEN_WIDTH, LINE_HEIGHT, TFT_BLACK);
        _tft.setTextSize(1);
        _tft.setCursor(0, LINE_LAMP_Y + 3);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.print("LAMP: ");
        if (!mqtt) {
            _tft.setTextColor(TFT_WHITE, TFT_BLACK);
            _tft.print("--");
        } else if (!hasLamp) {
            _tft.setTextColor(TFT_WHITE, TFT_BLACK);
            _tft.print("?");
        } else {
            _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            _tft.print(lamp);
        }
        _lastHasLamp = hasLamp;
        _lastLamp    = lamp;
    }

    _lastWifi = wifi;
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