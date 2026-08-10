#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include "WiFiManager.h"
#include "WiFiCredentials.h"
#include "LedManager.h"
#include "MqttManager.h"
#include "MqttCredentials.h"
#include "StateManager.h"

class AppState {
public:
    AppState();
    void begin();

    // --- Сканирование ---
    bool startScan();
    int getNetworkCount() const;
    NetworkInfo getNetwork(int index) const;
    bool hasScanResult() const;

    // --- Подключение ---
    bool connectToNetwork(const String& ssid, const String& password);
    bool connectToSavedNetwork(int index);
    bool saveCurrentNetwork();
    bool deleteSavedNetwork(int index);

    // --- Сохранённые сети ---
    int getSavedCount();
    String getSavedSSID(int index);
    String getSavedPassword(const String& ssid);
    bool hasSavedPassword(const String& ssid);
    void printSavedNetworks();

    // --- Статус ---
    AppStateEnum getState() const;
    String getStatusString() const;
    bool isConnected() const;
    String getCurrentSSID() const;
    IPAddress getIP() const;
    const char* getEncryptionType(uint8_t encType) const;

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

    // Доступ к менеджеру состояния (для Menu)
    StateManager& getStateManager() { return _stateManager; }

private:
    static const int MAX_NETWORKS = 50;

    StateManager _stateManager;
    WiFiManager _wifiManager;
    WiFiCredentials _credentials;
    String _lastSSID;
    String _lastPassword;
    NetworkInfo _networks[MAX_NETWORKS];
    LedManager _led;
    MqttManager _mqttManager;
    MqttCredentials _mqttCredentials;
    int _networkCount;
    bool _hasScanResult;

    // Приватный метод для установки состояния и обновления светодиода
    void setState(AppStateEnum newState);

    bool connect(const String& ssid, const String& password);
    void autoConnect();
    void handleMqttMessage(const String& topic, const String& payload);
};

#endif