#include "Menu.h"
#include "AppState.h"

AppState appState;
Menu menu(appState);

void setup() {
    Serial.begin(115200);
    delay(100);

    // Инициализация светодиода
    appState.beginLed(8, true);

    // Инициализация AppState (автоподключение Wi-Fi)
    appState.begin();

    // Инициализация меню
    menu.begin();
    appState.beginMqtt();  

void loop() {
    menu.update();
    appState.update();
}