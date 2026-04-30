// WiFiManager.h (обновлённый)
#ifndef WiFiManager_H
#define WiFiManager_H

#include <WiFi.h>
#include <Preferences.h>
#include "WiFiScanner.h"
#include "UserInput.h"

class WiFiManager {
private:
  Preferences preferences;
  const char* namespaceName = "wifi_config";
  String ssid;
  String password;
  WiFiScanner scanner;
  UserInput userInput;

public:
  WiFiManager();
  bool begin();
  bool connectWithInput();
  bool connect(const char* ssid, const char* password);
  bool saveCredentials(const char* ssid, const char* password);
  bool loadCredentials();
  void forgetCredentials();
  bool isConnected();
  String getSSID();
  String getIPAddress();

private:
  bool interactiveSetup();
};

#endif
