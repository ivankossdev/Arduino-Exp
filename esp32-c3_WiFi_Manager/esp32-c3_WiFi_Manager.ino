#include "Menu.h"
#include "AppState.h"

AppState appState;
Menu menu(appState);

void setup() {
    Serial.begin(115200);
    delay(100);

    // Инициализация светодиода (пин 8 – для примера)
    appState.beginLed(8, true);

    // Инициализация AppState (автоподключение к Wi-Fi)
    appState.begin();

    // Инициализация меню
    menu.begin();

    // Инициализация MQTT
    appState.beginMqtt(
        "192.168.0.239",        // IP брокера
        1883,                   // порт
        "esp32_user",           // логин
        "1234",                 // пароль
        "home/lamp/command",    // командный топик
        "home/lamp/status"      // топик состояния
    );
}

void loop() {
    menu.update();      // обработка ввода с Serial
    appState.update();  // обновление светодиода и MQTT
}