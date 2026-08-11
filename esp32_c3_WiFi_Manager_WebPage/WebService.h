#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include <Arduino.h>
#include <WebServer.h>
#include "AppState.h"

class WebService {
public:
    WebService(AppState& appState);
    ~WebService();

    void begin(bool apMode);
    void handleClient();   // вызывать в loop()

private:
    AppState& _appState;
    WebServer _server;
    bool _apMode;

    void setupRoutes();
    void handleRoot();
    void handleScan();
    void handleConnect();
    void handleSaved();
    void handleDelete();
    void handleStatus();
    void handleLed();
    void handleMqttGet();
    void handleMqttPost();
    void handleReset();
    void handleReboot();
    void handleNotFound();

    String getScanJson();
    String getSavedJson();
    String getMqttSettingsJson();
};

#endif