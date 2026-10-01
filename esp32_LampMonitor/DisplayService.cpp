// ============================================================
// DisplayService — реализация для TFT_eSPI (ST7789V)
// ============================================================
// ИЗМЕНЕНО: все вызовы Adafruit_SSD1306 заменены на TFT_eSPI
// ИЗМЕНЕНО: clearDisplay() → fillScreen(TFT_BLACK)
// ИЗМЕНЕНО: display() → не требуется, TFT_eSPI пишет сразу
// ИЗМЕНЕНО: println() → tft.println() с явным setCursor()
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
      _tft(),                  // ИЗМЕНЕНО: TFT_eSPI инициализируется без параметров
      _lastUpdate(0),
      _updateInterval(2000),
      _currentState(AppStateEnum::IDLE)
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

    // ИЗМЕНЕНО: вместо Wire.begin(4, 5) — инициализация TFT_eSPI
    _tft.init();
    _tft.setRotation(1);              // ИЗМЕНЕНО: альбомная ориентация (240x135)
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(2);              // ИЗМЕНЕНО: крупнее, т.к. экран больше
    _tft.setCursor(0, 0);
    _tft.println("ESP32");
    _tft.println("Load...");

    Serial.println("[TFT] OK");
    inited = true;
    return true;
}

void DisplayService::update() {
    unsigned long now = millis();
    if (now - _lastUpdate >= _updateInterval) {
        _lastUpdate = now;
        drawScreen();
    }
}

void DisplayService::handleStateChange(AppStateEnum newState) {
    _currentState = newState;
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
}

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

void DisplayService::drawClientMode() {
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_GREEN, TFT_BLACK);
    _tft.setTextSize(2);
    _tft.setCursor(0, 0);
    _tft.println("=== CLIENT ===");

    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextSize(1);

    if (_wifiService.isConnected()) {
        _tft.print("WiFi: ");
        _tft.println(_wifiService.getCurrentSSID());
        _tft.print("IP: ");
        _tft.println(_wifiService.getIP().toString());
        _tft.print("RSSI: ");
        _tft.print(WiFi.RSSI());
        _tft.println(" dBm");
    } else {
        _tft.println("WiFi: NO");
    }

    if (!_mqttService.isConnected()) {
        _tft.setTextColor(TFT_RED, TFT_BLACK);
        _tft.println("MQTT: NO (LOST)");
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.println("LAMP: --");
    } else if (!_lampStateService.hasData()) {
        _tft.println("MQTT: OK");
        _tft.println("LAMP: ?");
    } else {
        _tft.println("MQTT: OK");
        _tft.print("LAMP: ");
        _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        _tft.println(_lampStateService.getPayload());
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