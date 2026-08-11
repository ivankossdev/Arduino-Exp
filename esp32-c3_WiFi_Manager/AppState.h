#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include "StateManager.h"
#include "WiFiService.h"      // новый
#include "LedManager.h"
#include "MqttManager.h"
#include "MqttCredentials.h"

class AppState {
public:
    AppState();
    void begin();

    // --- Сканирование (делегирует WiFiService) ---
    bool startScan() { return _wifiService.startScan(); }
    int getNetworkCount() const { return _wifiService.getNetworkCount(); }
    NetworkInfo getNetwork(int index) const { return _wifiService.getNetwork(index); }
    bool hasScanResult() const { return _wifiService.hasScanResult(); }

    // --- Подключение (делегирует WiFiService) ---
    bool connectToNetwork(const String& ssid, const String& password) {
        return _wifiService.connectToNetwork(ssid, password);
    }
    bool connectToSavedNetwork(int index) {
        return _wifiService.connectToSavedNetwork(index);
    }
    bool saveCurrentNetwork() { return _wifiService.saveCurrentNetwork(); }
    bool deleteSavedNetwork(int index) { return _wifiService.deleteSavedNetwork(index); }

    // --- Сохранённые сети (делегирует WiFiService) ---
    int getSavedCount() { return _wifiService.getSavedCount(); }
    String getSavedSSID(int index) { return _wifiService.getSavedSSID(index); }
    String getSavedPassword(const String& ssid) { return _wifiService.getSavedPassword(ssid); }
    bool hasSavedPassword(const String& ssid) { return _wifiService.hasSavedPassword(ssid); }
    void printSavedNetworks() { _wifiService.printSavedNetworks(); }

    // --- Статус (делегирует WiFiService) ---
    AppStateEnum getState() const { return _stateManager.getState(); }
    String getStatusString() const;
    bool isConnected() const { return _wifiService.isConnected(); }
    String getCurrentSSID() const { return _wifiService.getCurrentSSID(); }
    IPAddress getIP() const { return _wifiService.getIP(); }
    const char* getEncryptionType(uint8_t encType) const {
        return _wifiService.getEncryptionType(encType);
    }

    // --- Светодиод ---
    bool beginLed(int pin, bool activeLow = true);
    void updateLed();

    // --- MQTT ---
    bool beginMqtt();
    bool beginMqtt(const String& server, int port,
                   const String& user, const String& password,
                   const String& cmdTopic, const String& stateTopic);
    bool configureMqtt(const String& server, int port,
                       const String& user, const String& password,
                       const String& cmdTopic, const String& stateTopic);
    bool saveMqttCredentials();
    bool loadMqttCredentials();
    MqttCredentials& getMqttCredentials() { return _mqttCredentials; }
    void updateMqtt();

    // Общий update
    void update();

    // Доступ к менеджеру состояния (для Menu, если потребуется)
    StateManager& getStateManager() { return _stateManager; }

private:
    StateManager _stateManager;
    WiFiService _wifiService;      // заменяет старые Wi-Fi члены
    LedManager _led;
    MqttManager _mqttManager;
    MqttCredentials _mqttCredentials;

    // Приватные методы, которые остались
    void handleMqttMessage(const String& topic, const String& payload);
};

#endif