#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"      // новый
#include "LedManager.h"

class AppState {
public:
    AppState();
    void begin();

    // --- Wi-Fi (делегирует WiFiService) ---
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

    // --- MQTT (делегирует MqttService) ---
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

    // Доступ к сервисам (для Menu, если потребуется)
    StateManager& getStateManager() { return _stateManager; }
    WiFiService& getWiFiService() { return _wifiService; }
    MqttService& getMqttService() { return _mqttService; }

private:
    StateManager _stateManager;
    LedManager _led;
    WiFiService _wifiService;
    MqttService _mqttService;    // новый член

    // Приватный метод для обработки MQTT-сообщений (регистрируем как колбэк)
    void handleMqttMessage(const String& topic, const String& payload);
};

#endif