// ============================================================
// DisplayService — реализация для TFT_eSPI (ST7789V)
// ============================================================
// ИЗМЕНЕНО: все вызовы Adafruit_SSD1306 заменены на TFT_eSPI
// ИЗМЕНЕНО: drawClientMode() перерисовывает экран ПОСТРОЧНО —
//           каждая строка (WiFi / IP / RSSI / MQTT / LAMP) сравнивается
//           со своим кэшем и обновляется только при изменении.
//           Это устраняет любое мерцание при смене одного значения.
// ============================================================

#include "DisplayService.h"

// Позиции строк в клиентском режиме (px, сверху вниз).
// Высота каждой строки — 14 px: 8 px текст (font size 1) + 6 px зазор.
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
      _updateInterval(2000),
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

    // ИЗМЕНЕНО: убрана проверка «есть ли изменения» здесь —
    // теперь это делает drawClientMode() построчно. В статичных
    // состояниях (AP / SCANNING / CONNECTING / ERROR) периодический
    // вызов drawScreen() не нужен — там контент не меняется, а
    // перерисовка была бы полной (fillScreen) и давала бы мерцание.
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
    _fullRedraw = false;
}

// ============================================================
// Статичные экраны: рисуются только при смене состояния,
// поэтому fillScreen здесь не даёт мерцания.
// ============================================================

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

// ============================================================
// Клиентский режим — построчное обновление.
//
// Полный fillScreen вызывается только когда:
//   - _fullRedraw = true (пришла смена состояния приложения), или
//   - изменился сам факт наличия Wi-Fi (был / пропал) — тогда
//     компоновка блока меняется и старые строки нужно стереть.
//
// В остальных случаях каждая строка проверяется отдельно и
// перерисовывается через маленький fillRect только если её
// значение действительно поменялось.
// ============================================================
void DisplayService::drawClientMode() {
    bool   wifi    = _wifiService.isConnected();
    bool   mqtt    = _mqttService.isConnected();
    bool   hasLamp = _lampStateService.hasData();
    String ssid    = wifi ? _wifiService.getCurrentSSID() : String("");
    String ip      = wifi ? _wifiService.getIP().toString() : String("");
    int    rssi    = wifi ? (WiFi.RSSI() / 5) * 5 : 0;
    String lamp    = hasLamp ? _lampStateService.getPayload() : String("");

    bool needFull = _fullRedraw || (_lastWifi != wifi);

    // --- Полная перерисовка: фон + заголовок + сброс кэшей строк ---
    if (needFull) {
        _tft.fillScreen(TFT_BLACK);
        _tft.setTextColor(TFT_GREEN, TFT_BLACK);
        _tft.setTextSize(2);
        _tft.setCursor(0, 0);
        _tft.println("=== CLIENT ===");

        // Сброс кэшей, чтобы все строки перерисовались ниже
        _lastSSID    = "";
        _lastIP      = "";
        _lastRSSI    = -999;
        _lastMqtt    = !mqtt;
        _lastHasLamp = !hasLamp;
        _lastLamp    = "";
    }

    // --- Строка WiFi ---
    if (needFull || _lastSSID != ssid) {
        _tft.fillRect(0, LINE_WIFI_Y, 240, LINE_HEIGHT, TFT_BLACK);
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

    // --- Строка IP ---
    if (needFull || _lastIP != ip) {
        _tft.fillRect(0, LINE_IP_Y, 240, LINE_HEIGHT, TFT_BLACK);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(1);
        _tft.setCursor(0, LINE_IP_Y + 3);
        if (wifi) {
            _tft.print("IP: ");
            _tft.print(ip);
        }
        _lastIP = ip;
    }

    // --- Строка RSSI ---
    if (needFull || _lastRSSI != rssi) {
        _tft.fillRect(0, LINE_RSSI_Y, 240, LINE_HEIGHT, TFT_BLACK);
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

    // --- Строка MQTT ---
    // Запоминаем факт изменения MQTT, чтобы ниже понять,
    // нужно ли перерисовать строку LAMP (её содержимое зависит от MQTT).
    bool mqttChanged = (needFull || _lastMqtt != mqtt);

    if (mqttChanged) {
        _tft.fillRect(0, LINE_MQTT_Y, 240, LINE_HEIGHT, TFT_BLACK);
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

    // --- Строка LAMP ---
    // Перерисовываем если:
    //  - сменилось MQTT (тогда меняется и отображение LAMP: "--", "?", значение)
    //  - сменилось наличие данных
    //  - сменилось само значение payload
    if (needFull || mqttChanged || _lastHasLamp != hasLamp || _lastLamp != lamp) {
        _tft.fillRect(0, LINE_LAMP_Y, 240, LINE_HEIGHT, TFT_BLACK);
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