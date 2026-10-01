// ============================================================
// WebService — веб-сервер (адаптация под ESP32)
// ============================================================
// ИЗМЕНЕНО: ESP8266WebServer → WebServer (ESP32 Core)
// ИЗМЕНЕНО: include <WebServer.h> убран отсюда — он уже подтянут
//           через AppState.h → DisplayService.h ДО TFT_eSPI.h,
//           что решает конфликт с FS_NO_GLOBALS
// ============================================================

#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include <Arduino.h>
#include "AppState.h"
// ИЗМЕНЕНО: убран #include <WebServer.h>
// Он уже подключён в DisplayService.h до TFT_eSPI.h — это критично
// для правильного порядка инициализации глобального типа FS.

class WebService {
public:
    WebService(AppState& appState);
    ~WebService();

    void begin(bool apMode);
    void handleClient();

private:
    AppState& _appState;
    WebServer _server;          // ИЗМЕНЕНО: было ESP8266WebServer

    bool _apMode;
    unsigned long _lastWifiOk = 0;
    static const unsigned long AP_FALLBACK_TIMEOUT_MS = 60000;

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