#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <Arduino.h>
#include "WiFiManager.h"
#include "WiFiCredentials.h"
#include "StateManager.h"

// Всё содержимое без изменений, кроме того, что WiFiManager.h теперь тянет ESP8266WiFi.
class WiFiService {
public:
    WiFiService(StateManager& stateManager);

    void begin();

    bool startScan();
    int getNetworkCount() const;
    NetworkInfo getNetwork(int index) const;
    bool hasScanResult() const;

    bool connectToNetwork(const String& ssid, const String& password);
    bool connectToSavedNetwork(int index);
    bool saveCurrentNetwork();
    bool deleteSavedNetwork(int index);

    int getSavedCount();
    String getSavedSSID(int index);
    String getSavedPassword(const String& ssid);
    bool hasSavedPassword(const String& ssid);
    void printSavedNetworks();

    bool isConnected() const;
    String getCurrentSSID() const;
    IPAddress getIP() const;
    const char* getEncryptionType(uint8_t encType) const;

    void setApCredentials(const String& ssid, const String& password);

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

    String _apSsid;
    String _apPassword;

    bool connect(const String& ssid, const String& password);
    void autoConnect();
    void setState(AppStateEnum newState);
};

#endif