#include "Menu.h"
#include "AppState.h"

AppState appState;
Menu menu(appState);

void setup() {
    Serial.begin(115200);
    appState.beginLed(8, true); // activeLow = true (обычно для встроенного)
    appState.begin();
    menu.begin();
}

void loop() {
    menu.update();
    appState.updateLed();
}