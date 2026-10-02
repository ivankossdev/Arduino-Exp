// ============================================================
// WebService — веб-сервер (ESP32)
// ============================================================
// ИЗМЕНЕНО: порт и таймаут fallback берутся из config.h
// ============================================================

#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include <Arduino.h>
#include "config.h"             // ИЗМЕНЕНО: константы вынесены сюда
#include "AppState.h"

class WebService {
public:
    WebService(AppState& appState);
    ~WebService();

    void begin(bool apMode);
    void handleClient();

private:
    AppState& _appState;
    WebServer _server;
    bool _apMode;

    unsigned long _lastWifiOk = 0;
    // ИЗМЕНЕНО: было static const unsigned long AP_FALLBACK_TIMEOUT_MS = 60000;
    static const unsigned long AP_FALLBACK_TIMEOUT_MS = WEB_AP_FALLBACK_TIMEOUT_MS;

    void setupRoutes();
    void startApMode();
    void checkWifiLoss();

    void handleRoot();
    void handleScan();
    void handleConnect();
    void handleSaved();
    void handleDelete();
    void handleStatus();
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