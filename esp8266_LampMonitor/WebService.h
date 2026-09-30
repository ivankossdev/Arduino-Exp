#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "AppState.h"

class WebService {
public:
    WebService(AppState& appState);
    ~WebService();

    void begin(bool apMode);
    void handleClient();

private:
    AppState& _appState;
    ESP8266WebServer _server;
    bool _apMode;

    // === НОВОЕ: время последней удачной проверки Wi-Fi ===
    // Используется в checkWifiLoss() — если Wi-Fi отсутствует дольше
    // AP_FALLBACK_TIMEOUT_MS, устройство перезагружается.
    unsigned long _lastWifiOk = 0;

    // Сколько миллисекунд терпим отсутствие Wi-Fi в клиентском режиме,
    // прежде чем перезагрузиться в поисках AP-fallback.
    static const unsigned long AP_FALLBACK_TIMEOUT_MS = 60000;

    void setupRoutes();

    // === НОВОЕ: поднимает AP-точку. Вынесено из begin() для переиспользования ===
    // Может вызываться многократно — если AP уже активен, softAP() просто
    // ничего не сделает.
    void startApMode();

    // === НОВОЕ: проверка потери Wi-Fi в клиентском режиме ===
    // Вызывается из handleClient(). При длительной потере — ESP.restart().
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