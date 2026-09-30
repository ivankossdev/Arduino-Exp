#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"
#include "LedService.h"       // === ИЗМЕНЕНО: было LedManager.h ===
#include "DisplayService.h"
#include "StatusBuilder.h"    // === НОВОЕ: класс-сборщик статуса ===

class AppState {
public:
    AppState();
    void begin();

    // --- Wi-Fi (делегирует WiFiService) — без изменений ---
    bool startScan() { return _wifiService.startScan(); }
    int getNetworkCount() const { return _wifiService.getNetworkCount(); }
    NetworkInfo getNetwork(int index) const { return _wifiService.getNetwork(index); }
    bool hasScanResult() const { return _wifiService.hasScanResult(); }

    bool connectToNetwork(const String& ssid, const String& password) {
        return _wifiService.connectToNetwork(ssid, password);
    }
    bool connectToSavedNetwork(int index) {
        return _wifiService.connectToSavedNetwork(index);
    }
    bool saveCurrentNetwork() { return _wifiService.saveCurrentNetwork(); }
    bool deleteSavedNetwork(int index) { return _wifiService.deleteSavedNetwork(index); }

    int getSavedCount() { return _wifiService.getSavedCount(); }
    String getSavedSSID(int index) { return _wifiService.getSavedSSID(index); }
    String getSavedPassword(const String& ssid) { return _wifiService.getSavedPassword(ssid); }
    bool hasSavedPassword(const String& ssid) { return _wifiService.hasSavedPassword(ssid); }
    void printSavedNetworks() { _wifiService.printSavedNetworks(); }

    AppStateEnum getState() const { return _stateManager.getState(); }
    String getStatusString() const;   // === ИЗМЕНЕНО: теперь делегат в StatusBuilder ===
    bool isConnected() const { return _wifiService.isConnected(); }
    String getCurrentSSID() const { return _wifiService.getCurrentSSID(); }
    IPAddress getIP() const { return _wifiService.getIP(); }
    const char* getEncryptionType(uint8_t encType) const {
        return _wifiService.getEncryptionType(encType);
    }

    // --- Светодиод === ИЗМЕНЕНО: делегаты в LedService ---
    // Публичный API сохранён, чтобы не трогать WebService и DisplayService.
    bool beginLed(int pin, bool activeLow = true);
    void updateLed();      // оставлен для совместимости; внутри — LedService::update()
    void setLed(bool on);  // делегат в LedService::set
    bool getLedState() const;  // делегат в LedService::isOn

    // --- MQTT (делегирует MqttService) — без изменений ---
    bool beginMqtt() { return _mqttService.begin(); }
    bool beginMqtt(const String& server, int port,
                   const String& user, const String& password,
                   const String& cmdTopic, const String& stateTopic) {
        return _mqttService.begin(server, port, user, password, cmdTopic, stateTopic);
    }
    bool configureMqtt(const String& server, int port,
                       const String& user, const String& password,
                       const String& cmdTopic, const String& stateTopic) {
        return _mqttService.configure(server, port, user, password, cmdTopic, stateTopic);
    }
    bool saveMqttCredentials() { return _mqttService.saveCredentials(); }
    bool loadMqttCredentials() { return _mqttService.loadCredentials(); }
    MqttCredentials& getMqttCredentials() { return _mqttService.getCredentials(); }
    void updateMqtt() { _mqttService.update(); }

    // Общий update
    void update();

    // Доступ к сервисам
    StateManager& getStateManager() { return _stateManager; }
    WiFiService& getWiFiService() { return _wifiService; }
    MqttService& getMqttService() { return _mqttService; }
    DisplayService& getDisplayService() { return _displayService; }

    // --- Получение статуса в JSON (делегат в StatusBuilder) ---
    String getStatusJson();

private:
    StateManager _stateManager;
    WiFiService _wifiService;
    MqttService _mqttService;
    LedService _ledService;          // === НОВОЕ: заменяет LedManager _led ===
    DisplayService _displayService;
    StatusBuilder _statusBuilder;    // === НОВОЕ ===

    // === УБРАНЫ поля retained-синхронизации (шаг 3) ===
    // _awaitingRetainedState, _connectedAt, _retainedWaitTimeout
    // теперь живут в LedService.

    // Роутер входящих MQTT-сообщений. Делегирует в LedService,
    // при необходимости будет роутить в другие сервисы.
    void handleMqttMessage(const String& topic, const String& payload);
};

#endif