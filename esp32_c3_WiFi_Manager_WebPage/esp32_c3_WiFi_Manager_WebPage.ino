#include "AppState.h"

AppState appState;

void setup() {
    Serial.begin(115200);
    delay(100);
    appState.beginLed(8, true);
    appState.begin();
    appState.beginMqtt();  
}

void loop() {
    appState.update();
}