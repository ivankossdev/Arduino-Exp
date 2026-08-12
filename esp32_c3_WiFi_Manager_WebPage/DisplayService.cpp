#include "DisplayService.h"

DisplayService::DisplayService(StateManager& stateManager, WiFiService& wifiService, MqttService& mqttService)
    : _stateManager(stateManager),
      _wifiService(wifiService),
      _mqttService(mqttService),
      _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET),
      _lastUpdate(0),
      _updateInterval(2000), // Обновление каждые 2 секунды
      _currentState(AppStateEnum::IDLE)
{
    // Регистрируем колбэк на изменение состояния
    _stateManager.setOnStateChange([this](AppStateEnum newState) {
        this->handleStateChange(newState);
    });
}

bool DisplayService::begin() {
    // Инициализация I2C (пины могут отличаться, для ESP32-C3 обычно SDA=4, SCL=5)
    // Wire.begin(4, 5);  // SDA, SCL

    if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("❌ OLED инициализация не удалась!");
        return false;
    }

    _display.clearDisplay();
    _display.setTextColor(SSD1306_WHITE);
    _display.setTextSize(1);
    _display.setCursor(0, 0);
    _display.println("ESP32");
    _display.println("Load...");
    _display.display();

    Serial.println("✅ OLED дисплей инициализирован");
    return true;
}

void DisplayService::update() {
    // Обновляем экран по таймеру (для отображения динамических данных)
    unsigned long now = millis();
    if (now - _lastUpdate >= _updateInterval) {
        _lastUpdate = now;
        drawScreen();
    }
}

void DisplayService::handleStateChange(AppStateEnum newState) {
    _currentState = newState;
    drawScreen(); // Сразу обновляем экран при смене состояния
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
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("=== AP MODE ===");
    _display.println("SSID: ESP32-Setup");
    _display.println("IP: 192.168.4.1");
    _display.println("Configure via");
    _display.println("web interface");
    _display.display();
}

void DisplayService::drawClientMode() {
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("=== CLIENT ===");

    if (_wifiService.isConnected()) {
        _display.print("WiFi: ");
        _display.println(_wifiService.getCurrentSSID());
        _display.print("IP: ");
        _display.println(_wifiService.getIP().toString());
        _display.print("RSSI: ");
        _display.print(WiFi.RSSI());
        _display.println(" dBm");
    } else {
        _display.println("WiFi: NO");
    }

    _display.print("MQTT: ");
    _display.println(_mqttService.isConnected() ? "OK" : "NO");

    _display.display();
}

void DisplayService::drawScanning() {
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("=== SCANNING ===");
    _display.println("Searching...");
    _display.println("Please wait");
    _display.display();
}

void DisplayService::drawConnecting() {
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("=== CONNECTING ===");
    _display.println("Connecting to");
    _display.println("Wi-Fi...");
    _display.display();
}

void DisplayService::drawError() {
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("!!! ERROR !!!");
    _display.println("Failed to");
    _display.println("connect to");
    _display.println("Wi-Fi");
    _display.display();
}