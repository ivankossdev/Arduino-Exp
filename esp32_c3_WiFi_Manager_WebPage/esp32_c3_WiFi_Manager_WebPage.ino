#include "AppState.h"
#include "WebService.h"

AppState appState;
WebService webService(appState);

void setup() {
    //Serial.begin(115200);
    // Инициализация светодиода (пин 8, activeLow = true)
    appState.beginLed(8, true);

    // Проверяем, есть ли сохранённые сети
    int savedCount = appState.getSavedCount();

    if (savedCount == 0) {
        // Первый запуск или нет сохранённых сетей — сразу переходим в режим AP
        Serial.println("ℹ️ Нет сохранённых сетей. Запуск в режиме AP.");
        webService.begin(true);
    } else {
        // Есть сохранённые сети — пробуем автоподключиться
        appState.begin(); // запускает автоподключение

        // Даём до 10 секунд на подключение
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
            // Подключено — клиентский режим
            webService.begin(false);
        } else {
            // Не удалось подключиться — режим AP
            Serial.println("⚠️ Не удалось подключиться к сохранённым сетям. Запуск в режиме AP.");
            webService.begin(true);
        }
    }

    // Попытка запуска MQTT (если сохранены настройки)
    appState.beginMqtt();
}

void loop() {
    appState.update();        // обновление MQTT и светодиода
    webService.handleClient(); // обработка HTTP-запросов
}