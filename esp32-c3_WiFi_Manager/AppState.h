#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include "WiFiManager.h"
#include "WiFiCredentials.h"
#include "LedManager.h"
#include "MqttManager.h"
#include "MqttCredentials.h"   // добавлено

enum class AppStateEnum {
    IDLE,
    SCANNING,
    CONNECTING,
    CONNECTED,
    AP_MODE,
    ERROR
};

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
    bool beginMqtt(); // использует сохранённые настройки (если есть)
    bool beginMqtt(const String& server, int port,
                   const String& user, const String& password,
                   const String& cmdTopic, const String& stateTopic); // совместимость
    bool configureMqtt(const String& server, int port,
                       const String& user, const String& password,
                       const String& cmdTopic, const String& stateTopic);
    bool saveMqttCredentials();
    bool loadMqttCredentials();
    MqttCredentials& getMqttCredentials() { return _mqttCredentials; } // для доступа в Menu
    void updateMqtt();

    // Общий update для всех подсистем
    void update();

private:
    static const int MAX_NETWORKS = 50;

    AppStateEnum _state;
    WiFiManager _wifiManager;
    WiFiCredentials _credentials;
    String _lastSSID;
    String _lastPassword;
    NetworkInfo _networks[MAX_NETWORKS];
    LedManager _led;
    MqttManager _mqttManager;
    MqttCredentials _mqttCredentials;   // член для хранения настроек MQTT
    int _networkCount;
    bool _hasScanResult;

    void setState(AppStateEnum newState);
    bool connect(const String& ssid, const String& password);
    void autoConnect();

    // Обработчик MQTT-сообщений
    void handleMqttMessage(const String& topic, const String& payload);
};

#endif