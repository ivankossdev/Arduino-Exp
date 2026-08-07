#include "Menu.h"
#include "AppState.h"

AppState appState;
Menu menu(appState);

void setup() {
    Serial.begin(115200);
    appState.begin();
    menu.begin();
}

void loop() {
    menu.update();
}