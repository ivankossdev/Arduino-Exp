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
  String serverURL;
  WiFiScanner scanner;
  UserInput userInput;
  bool needsSetup = false;  // Флаг: нужны ли настройки

public:
  WiFiManager();
  bool begin();
  bool connectWithInput();
  bool connect(const char* ssid, const char* password);
  bool saveCredentials(const char* ssid, const char* password, const char* url);
  bool loadCredentials();
  void forgetCredentials();
  bool isConnected();
  String getSSID();
  String getIPAddress();
  String getServerURL();
  bool requiresSetup();  

private:
  bool interactiveSetup();
  bool checkForSetupMode(); 
};

#endif
