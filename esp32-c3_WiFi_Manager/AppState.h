#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include "WiFiManager.h"
#include "WiFiCredentials.h"
#include "LedManager.h"

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
    int getNetworkCount() const;          // этот метод не вызывает _credentials, можно оставить const
    NetworkInfo getNetwork(int index) const;
    bool hasScanResult() const;

    // --- Подключение ---
    bool connectToNetwork(const String& ssid, const String& password);
    bool connectToSavedNetwork(int index);
    bool saveCurrentNetwork();
    bool deleteSavedNetwork(int index);

    // --- Сохранённые сети ---
    int getSavedCount();                  // убрали const
    String getSavedSSID(int index);       // убрали const
    String getSavedPassword(const String& ssid); // убрали const
    bool hasSavedPassword(const String& ssid);   // убрали const
    void printSavedNetworks();            // убрали const

    // --- Статус ---
    AppStateEnum getState() const;
    String getStatusString() const;
    bool isConnected() const;
    String getCurrentSSID() const;
    IPAddress getIP() const;
    const char* getEncryptionType(uint8_t encType) const; // делегирует WiFiManager, который имеет const-метод

    // --- Светодиод ---  
    bool beginLed(int pin, bool activeLow = true);
    void updateLed();

private:
    static const int MAX_NETWORKS = 50;

    AppStateEnum _state;
    WiFiManager _wifiManager;
    WiFiCredentials _credentials;
    String _lastSSID;
    String _lastPassword;
    NetworkInfo _networks[MAX_NETWORKS];
    LedManager _led; 
    int _networkCount;
    bool _hasScanResult;

    void setState(AppStateEnum newState);
    bool connect(const String& ssid, const String& password);
    void autoConnect();
};

#endif