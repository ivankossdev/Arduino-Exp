// ============================================================
// WiFiService — управление Wi-Fi (ESP32)
// ============================================================
// ИЗМЕНЕНО: MAX_NETWORKS перенесён в config.h как WIFI_SCAN_MAX_NETWORKS
// ИЗМЕНЕНО: добавлен #include "config.h"
// ============================================================

#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <Arduino.h>
#include "config.h"             // ИЗМЕНЕНО: константы вынесены сюда
#include "WiFiManager.h"
#include "WiFiCredentials.h"
#include "StateManager.h"

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
    // ИЗМЕНЕНО: было static const int MAX_NETWORKS = 50;
    // Теперь берём из config.h как WIFI_SCAN_MAX_NETWORKS.
    static const int MAX_NETWORKS = WIFI_SCAN_MAX_NETWORKS;

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