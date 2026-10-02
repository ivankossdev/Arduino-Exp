// ============================================================
// ESP32 Lamp Monitor для TTGO T-Display v1.1
// ============================================================
// ИЗМЕНЕНО: таймауты и дефолтный state-топик берутся из config.h
// ============================================================

#include "config.h"            // ИЗМЕНЕНО: подключены константы
#include "AppState.h"
#include "WebService.h"

AppState appState;
WebService webService(appState);

void setup() {
    Serial.begin(SERIAL_BAUD);
    // ИЗМЕНЕНО: было delay(100)
    delay(STARTUP_DELAY_MS);
    Serial.println();
    Serial.println("=== ESP32 Lamp Monitor (TTGO T-Display) ===");

    // Подсветка TFT — пин GPIO4 на TTGO T-Display
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);

    MqttCredentials& creds = appState.getMqttCredentials();
    if (creds.getStateTopic().length() == 0) {
        // ИЗМЕНЕНО: было "home/lamp/status"
        creds.setStateTopic(MQTT_DEFAULT_STATE_TOPIC);
        creds.save();
    }

    if (!appState.getDisplayService().begin()) {
        Serial.println("[ERR] TFT init failed");
    }

    int savedCount = appState.getSavedCount();

    if (savedCount == 0) {
        Serial.println("[WiFi] Нет сохранённых сетей — AP mode");
        webService.begin(true);
    } else {
        appState.begin();

        unsigned long start = millis();
        bool connected = false;
        while (millis() - start < WIFI_BOOT_WAIT_MS) {   // ИЗМЕНЕНО: было 10000
            if (appState.isConnected()) {
                connected = true;
                break;
            }
            delay(WIFI_BOOT_POLL_MS);                    // ИЗМЕНЕНО: было 200
        }

        if (connected) {
            webService.begin(false);
        } else {
            Serial.println("[WiFi] Автоподключение не удалось — AP mode");
            webService.begin(true);
        }
    }

    appState.beginMqtt();
}

void loop() {
    appState.update();
    webService.handleClient();
}