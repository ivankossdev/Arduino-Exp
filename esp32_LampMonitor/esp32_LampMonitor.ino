// ============================================================
// ESP32 Lamp Monitor для TTGO T-Display v1.1
// ============================================================
// ИЗМЕНЕНО: переименован из esp8266_LampMonitor.ino (платформа ESP32)
// ИЗМЕНЕНО: добавлено управление подсветкой TFT (GPIO4)
// ============================================================

#include "AppState.h"
#include "WebService.h"

AppState appState;
WebService webService(appState);

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println();
    Serial.println("=== ESP32 Lamp Monitor (TTGO T-Display) ===");

    // ИЗМЕНЕНО: инициализация подсветки дисплея TFT
    // На TTGO T-Display подсветка управляется пином GPIO4.
    // Без этого экран останется чёрным.
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);

    // Если state-топик не сохранён — ставим home/lamp/status по умолчанию
    MqttCredentials& creds = appState.getMqttCredentials();
    if (creds.getStateTopic().length() == 0) {
        creds.setStateTopic("home/lamp/status");
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
        while (millis() - start < 10000) {
            if (appState.isConnected()) {
                connected = true;
                break;
            }
            delay(200);
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