// ============================================================
// DisplayService — реализация для TFT_eSPI (ST7789V)
// ============================================================
// ИЗМЕНЕНО: все вызовы Adafruit_SSD1306 заменены на TFT_eSPI
// ИЗМЕНЕНО: clearDisplay() → fillScreen(TFT_BLACK)
// ИЗМЕНЕНО: display() → не требуется, TFT_eSPI пишет сразу
// ИЗМЕНЕНО: реализовано частичное обновление — устраняет мерцание
// ============================================================

#include "DisplayService.h"

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
      _updateInterval(2000),
      _currentState(AppStateEnum::IDLE),
      _fullRedraw(true),      // первый рендер — всегда полный
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
    _tft.setRotation(1);              // альбомная ориентация (240x135)
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("ESP32");
    _tft.println("Load...");

    Serial.println("[TFT] OK");
    inited = true;
    return true;
}

void DisplayService::update() {
    unsigned long now = millis();
    if (now - _lastUpdate < _updateInterval) return;
    _lastUpdate = now;

    // === НОВОЕ: если ничего не изменилось — вообще не трогаем экран ===
    // В статичных состояниях (AP/SCANNING/CONNECTING/ERROR) контент не меняется,
    // поэтому пропускаем перерисовку без дополнительных проверок.
    if (!_fullRedraw) {
        if (_currentState != AppStateEnum::IDLE &&
            _currentState != AppStateEnum::CONNECTED) {
            return;
        }

        // В клиентском режиме сравниваем текущие значения с последними отрисованными
        bool   wifi    = _wifiService.isConnected();
        bool   mqtt    = _mqttService.isConnected();
        bool   hasLamp = _lampStateService.hasData();
        String ssid    = wifi ? _wifiService.getCurrentSSID() : String("");
        String ip      = wifi ? _wifiService.getIP().toString() : String("");
        // Квантуем RSSI до 5 dBm — иначе он «дышит» на ±1-2 и дёргает перерисовку
        int    rssi    = wifi ? (WiFi.RSSI() / 5) * 5 : 0;
        String lamp    = hasLamp ? _lampStateService.getPayload() : String("");

        if (_lastWifi    == wifi    &&
            _lastSSID    == ssid    &&
            _lastIP      == ip      &&
            _lastRSSI    == rssi    &&
            _lastMqtt    == mqtt    &&
            _lastHasLamp == hasLamp &&
            _lastLamp    == lamp) {
            return;  // ничего не поменялось — выходим, экран не трогаем
        }
    }

    drawScreen();
}

void DisplayService::handleStateChange(AppStateEnum newState) {
    _currentState = newState;
    _fullRedraw = true;   // смена состояния → полная перерисовка
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
    _fullRedraw = false;  // сбрасываем флаг после отрисовки
}

// === Все «статичные» экраны делают fillScreen — они вызываются
// только при смене состояния, поэтому мерцание здесь не критично. ===

void DisplayService::drawApMode() {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_CYAN, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("=== AP MODE ===");
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(1);
    _tft.println("SSID: ESP32-Setup");
    _tft.println("IP: 192.168.4.1");
    _tft.println("Configure via");
    _tft.println("web interface");
}

// === НОВОЕ: клиентский режим рисуется частично ===
// Заголовок и статичная часть — только при _fullRedraw.
// WiFi-блок и MQTT/LAMP-блок — только если их значения изменились.
void DisplayService::drawClientMode() {
    bool   wifi    = _wifiService.isConnected();
    bool   mqtt    = _mqttService.isConnected();
    bool   hasLamp = _lampStateService.hasData();
    String ssid    = wifi ? _wifiService.getCurrentSSID() : String("");
    String ip      = wifi ? _wifiService.getIP().toString() : String("");
    int    rssi    = wifi ? (WiFi.RSSI() / 5) * 5 : 0;
    String lamp    = hasLamp ? _lampStateService.getPayload() : String("");

    // Полная перерисовка нужна, если:
    // - сменилось состояние приложения (_fullRedraw выставлен выше)
    // - изменилось наличие Wi-Fi (был / не был) — компоновка другая
    bool needFull = _fullRedraw || (_lastWifi != wifi);

    if (needFull) {
        _tft.fillScreen(TFT_BLACK);
        _tft.setTextColor(TFT_GREEN, TFT_BLACK);
        _tft.setTextSize(2);
        _tft.setCursor(0, 0);
        _tft.println("=== CLIENT ===");

        // Сброс кэша, чтобы блоки ниже перерисовались
        _lastSSID    = "";
        _lastIP      = "";
        _lastRSSI    = -999;
        _lastMqtt    = !mqtt;
        _lastHasLamp = !hasLamp;
        _lastLamp    = "";
    }

    // --- Блок Wi-Fi (3 строки, y = 24..78) ---
    if (needFull || _lastSSID != ssid || _lastIP != ip || _lastRSSI != rssi) {
        _tft.fillRect(0, 24, 240, 54, TFT_BLACK);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(1);
        _tft.setCursor(0, 24);
        if (wifi) {
            _tft.print("WiFi: ");
            _tft.println(ssid);
            _tft.print("IP: ");
            _tft.println(ip);
            _tft.print("RSSI: ");
            _tft.print(rssi);
            _tft.println(" dBm");
        } else {
            _tft.println("WiFi: NO");
        }
        _lastSSID = ssid;
        _lastIP   = ip;
        _lastRSSI = rssi;
    }

    // --- Блок MQTT / LAMP (y = 82..135) ---
    if (needFull || _lastMqtt != mqtt || _lastHasLamp != hasLamp || _lastLamp != lamp) {
        _tft.fillRect(0, 82, 240, 53, TFT_BLACK);
        _tft.setTextSize(1);
        _tft.setCursor(0, 82);
        if (!mqtt) {
            _tft.setTextColor(TFT_RED, TFT_BLACK);
            _tft.println("MQTT: NO (LOST)");
            _tft.setTextColor(TFT_WHITE, TFT_BLACK);
            _tft.println("LAMP: --");
        } else if (!hasLamp) {
            _tft.setTextColor(TFT_WHITE, TFT_BLACK);
            _tft.println("MQTT: OK");
            _tft.println("LAMP: ?");
        } else {
            _tft.setTextColor(TFT_WHITE, TFT_BLACK);
            _tft.println("MQTT: OK");
            _tft.print("LAMP: ");
            _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            _tft.println(lamp);
        }
        _lastMqtt    = mqtt;
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