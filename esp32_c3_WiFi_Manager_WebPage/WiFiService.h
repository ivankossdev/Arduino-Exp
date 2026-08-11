#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <Arduino.h>
#include "WiFiManager.h"
#include "WiFiCredentials.h"
#include "StateManager.h"

class WiFiService {
public:
    // Конструктор теперь принимает только StateManager
    WiFiService(StateManager& stateManager);

    void begin(); // автоподключение

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

    // --- Статус Wi-Fi ---
    bool isConnected() const;
    String getCurrentSSID() const;
    IPAddress getIP() const;
    const char* getEncryptionType(uint8_t encType) const;

private:
    static const int MAX_NETWORKS = 50;

    StateManager& _stateManager;
    WiFiManager _wifiManager;
    WiFiCredentials _credentials;
    String _lastSSID;
    String _lastPassword;
    NetworkInfo _networks[MAX_NETWORKS];
    int _networkCount;
    bool _hasScanResult;

    // Приватные методы
    bool connect(const String& ssid, const String& password);
    void autoConnect();
    void setState(AppStateEnum newState); // только меняет состояние, без светодиода
};

#endif