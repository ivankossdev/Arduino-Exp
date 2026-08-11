#include "AppState.h"
#include "WebService.h"

AppState appState;
WebService webService(appState);

void setup() {
    appState.beginLed(8, true);
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
        webService.begin(true);
    }

    appState.beginMqtt();
}

void loop() {
    appState.update();
    webService.handleClient();  // обработка входящих запросов
}