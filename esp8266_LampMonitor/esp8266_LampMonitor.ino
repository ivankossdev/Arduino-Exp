// === ИЗМЕНЕНО: точка входа адаптирована под ESP8266 ===
// === УДАЛЕНО: инициализация светодиода (beginLed) ===
// === ДОБАВЛЕНО: default state-топик при первом запуске ===

#include "AppState.h"
#include "WebService.h"

AppState appState;
WebService webService(appState);

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("\n=== ESP8266 Lamp Monitor ===");

    // === ДОБАВЛЕНО: если state-топик не сохранён — ставим home/lamp/status ===
    MqttCredentials& creds = appState.getMqttCredentials();
    if (creds.getStateTopic().length() == 0) {
        creds.setStateTopic("home/lamp/status");
        creds.save();
        Serial.println("ℹ️ Установлен state-топик по умолчанию: home/lamp/status");
    }

    // Инициализация дисплея
    if (!appState.getDisplayService().begin()) {
        Serial.println("⚠️ Дисплей не инициализирован");
    }

    // Проверяем, есть ли сохранённые Wi-Fi сети
    int savedCount = appState.getSavedCount();

    if (savedCount == 0) {
        Serial.println("ℹ️ Нет сохранённых сетей. Запуск в режиме AP.");
        webService.begin(true);
    } else {
        appState.begin(); // автоподключение

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
            Serial.println("⚠️ Не удалось подключиться. Запуск в режиме AP.");
            webService.begin(true);
        }
    }

    // Попытка запуска MQTT (если сохранены настройки)
    appState.beginMqtt();
}

void loop() {
    // === ИЗМЕНЕНО: убран отдельный вызов getDisplayService().update() — он уже внутри AppState::update() ===
    appState.update();
    webService.handleClient();
}