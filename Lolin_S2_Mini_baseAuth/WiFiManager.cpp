// WiFiManager.cpp (обновлённый)
#include "WiFiManager.h"

WiFiManager::WiFiManager() {}

bool WiFiManager::begin() {
  preferences.begin(namespaceName, false);

  // Сначала пытаемся подключиться с сохранёнными настройками
  if (loadCredentials()) {
    Serial.println("Loading saved WiFi credentials...");
    if (connect(ssid.c_str(), password.c_str())) {
      return true;
    }
  }

  // Если не получилось — запускаем интерактивный режим
  Serial.println("Starting interactive setup...");
  return interactiveSetup();
}

bool WiFiManager::interactiveSetup() {
  scanner.scanNetworks();

  String userSSID = userInput.readSSID();

  // Если введён номер из списка — берём SSID оттуда
  int index = userSSID.toInt() - 1;
  if (index >= 0 && index < scanner.getNetworkCount()) {
    ssid = scanner.getSSID(index);
  } else {
    ssid = userSSID;
  }

  password = userInput.readPassword();

  if (!userInput.confirmSelection()) {
    Serial.println("Setup cancelled");
    return false;
  }

  // Пытаемся подключиться
  if (connect(ssid.c_str(), password.c_str())) {
    saveCredentials(ssid.c_str(), password.c_str());
    return true;
  }

  Serial.println("Connection failed");
  return false;
}

bool WiFiManager::connect(const char* ssid, const char* password) {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nFailed to connect to WiFi");
    return false;
  }
}

bool WiFiManager::saveCredentials(const char* ssid, const char* password) {
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  this->ssid = ssid;
  this->password = password;
  Serial.println("WiFi credentials saved to flash");
  return true;
}

bool WiFiManager::loadCredentials() {
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");

  if (ssid.length() > 0 && password.length() > 0) {
    return true;
  }
  return false;
}

void WiFiManager::forgetCredentials() {
  preferences.remove("ssid");
  preferences.remove("password");
  ssid = "";
  password = "";
  Serial.println("WiFi credentials removed from flash");
}

bool WiFiManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getSSID() {
  return ssid;
}

String WiFiManager::getIPAddress() {
  if (isConnected()) {
    return WiFi.localIP().toString();
  }
  return "Not connected";
}

bool WiFiManager::connectWithInput() {
  return interactiveSetup();
}
