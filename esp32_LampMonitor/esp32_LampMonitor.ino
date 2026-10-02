// ============================================================
// ESP32 Lamp Monitor для TTGO T-Display v1.1
// ============================================================
// ИЗМЕНЕНО: пользовательские сообщения переведены на LOG/LOG_LN.
//           Ошибка инициализации TFT печатается через LOG_ALWAYS
//           — она показывается даже при полностью выключенных логах.
// ============================================================

#include "config.h"
#include "log.h"              // ИЗМЕНЕНО: макросы логирования
#include "AppState.h"
#include "WebService.h"

AppState appState;
WebService webService(appState);

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(STARTUP_DELAY_MS);
    Serial.println();
    LOG_LN(LOG_CAT_SYS, "=== ESP32 Lamp Monitor (TTGO T-Display) ===");

    // Подсветка TFT — пин GPIO4 на TTGO T-Display
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);

    MqttCredentials& creds = appState.getMqttCredentials();
    if (creds.getStateTopic().length() == 0) {
        creds.setStateTopic(MQTT_DEFAULT_STATE_TOPIC);
        creds.save();
    }

    if (!appState.getDisplayService().begin()) {
        // ИЗМЕНЕНО: критичная ошибка — печатается всегда, вне маски
        LOG_ALWAYS("[ERR] TFT init failed\n");
    }

    int savedCount = appState.getSavedCount();

    if (savedCount == 0) {
        LOG_LN(LOG_CAT_WIFI, "[WiFi] Нет сохранённых сетей — AP mode");
        webService.begin(true);
    } else {
        appState.begin();

        unsigned long start = millis();
        bool connected = false;
        while (millis() - start < WIFI_BOOT_WAIT_MS) {
            if (appState.isConnected()) {
                connected = true;
                break;
            }
            delay(WIFI_BOOT_POLL_MS);
        }

        if (connected) {
            webService.begin(false);
        } else {
            LOG_LN(LOG_CAT_WIFI, "[WiFi] Автоподключение не удалось — AP mode");
            webService.begin(true);
        }
    }

    appState.beginMqtt();
}

void loop() {
    appState.update();
    webService.handleClient();
}