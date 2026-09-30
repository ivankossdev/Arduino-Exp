#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include <Arduino.h>
#include <ESP8266WebServer.h>   // === ИЗМЕНЕНО: было <WebServer.h> ===
#include "AppState.h"

class WebService {
public:
    WebService(AppState& appState);
    ~WebService();

    void begin(bool apMode);
    void handleClient();

private:
    AppState& _appState;
    ESP8266WebServer _server;   // === ИЗМЕНЕНО ===
    bool _apMode;

    void setupRoutes();
    void handleRoot();
    void handleScan();
    void handleConnect();
    void handleSaved();
    void handleDelete();
    void handleStatus();
    // === УДАЛЕНО: handleLed — нет управления светодиодом ===
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