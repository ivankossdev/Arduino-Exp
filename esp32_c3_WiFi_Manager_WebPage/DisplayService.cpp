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
    _display.println("Загрузка...");
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
    _display.println("=== AP РЕЖИМ ===");
    _display.println("SSID: ESP32-Setup");
    _display.println("IP: 192.168.4.1");
    _display.println("Настройте через");
    _display.println("веб-интерфейс");
    _display.display();
}

void DisplayService::drawClientMode() {
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("=== КЛИЕНТ ===");

    // Wi-Fi статус
    if (_wifiService.isConnected()) {
        _display.print("WiFi: ");
        _display.println(_wifiService.getCurrentSSID());
        _display.print("IP: ");
        _display.println(_wifiService.getIP().toString());

        // RSSI (уровень сигнала)
        _display.print("RSSI: ");
        _display.print(WiFi.RSSI());
        _display.println(" dBm");
    } else {
        _display.println("WiFi: НЕТ");
    }

    // MQTT статус
    _display.print("MQTT: ");
    _display.println(_mqttService.isConnected() ? "OK" : "НЕТ");

    // Состояние светодиода
    // Можно добавить, если есть доступ к LedManager через AppState
    // _display.print("LED: ");
    // _display.println(_appState.getLedState() ? "ON" : "OFF");

    _display.display();
}

void DisplayService::drawScanning() {
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("=== СКАНИРОВАНИЕ ===");
    _display.println("Поиск сетей...");
    _display.println("Подождите...");
    _display.display();
}

void DisplayService::drawConnecting() {
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("=== ПОДКЛЮЧЕНИЕ ===");
    _display.println("Подключение к");
    _display.println("Wi-Fi сети...");
    _display.display();
}

void DisplayService::drawError() {
    _display.clearDisplay();
    _display.setCursor(0, 0);
    _display.println("!!! ОШИБКА !!!");
    _display.println("Не удалось");
    _display.println("подключиться к");
    _display.println("Wi-Fi сети");
    _display.display();
}