#include "AppState.h"
#include "WebService.h"

AppState appState;
WebService webService(appState);

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println();
    Serial.println("=== ESP8266 Lamp Monitor ===");

    // Если state-топик не сохранён — ставим home/lamp/status по умолчанию
    MqttCredentials& creds = appState.getMqttCredentials();
    if (creds.getStateTopic().length() == 0) {
        creds.setStateTopic("home/lamp/status");
        creds.save();
    }

    if (!appState.getDisplayService().begin()) {
        Serial.println("[ERR] OLED init failed");
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