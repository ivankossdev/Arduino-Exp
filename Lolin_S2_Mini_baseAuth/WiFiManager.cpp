#include "WiFiManager.h"

WiFiManager::WiFiManager() {}

bool WiFiManager::begin() {
  Serial.println("WiFi init");
  preferences.begin(namespaceName, false);

  // Проверяем, хочет ли пользователь изменить настройки
  if (checkForSetupMode()) {
    // Пользователь хочет изменить настройки — запускаем интерактивный режим
    return interactiveSetup();
  }

  // Загружаем сохранённые настройки
  if (loadCredentials()) {
    Serial.println("Loading saved WiFi credentials...");
    if (connect(ssid.c_str(), password.c_str())) {
      return true;
    }
  }

  // Если не получилось подключиться — запускаем интерактивный режим
  Serial.println("Starting interactive setup...");
  return interactiveSetup();
}


bool WiFiManager::interactiveSetup() {
  Serial.println("\n=== WiFi Setup Mode ===");

  // Ввод SSID с выбором из списка или вручную
  ssid = userInput.readSSID(scanner);

  // Ввод пароля
  password = userInput.readPassword();

  // Ввод URL сервера
  serverURL = userInput.readServerURL();

  // Подтверждение
  if (!userInput.confirmSelection()) {
    Serial.println("Setup cancelled");
    return false;
  }

  // Подключение
  Serial.println("\nAttempting to connect...");
  if (connect(ssid.c_str(), password.c_str())) {
    saveCredentials(ssid.c_str(), password.c_str(), serverURL.c_str());
    Serial.println("Settings saved and connected successfully!");
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

bool WiFiManager::saveCredentials(const char* ssid, const char* password, const char* url) {
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.putString("server_url", url);  // Сохраняем URL
  this->ssid = ssid;
  this->password = password;
  this->serverURL = url;
  Serial.println("WiFi credentials and server URL saved to flash");
  return true;
}

bool WiFiManager::loadCredentials() {
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  serverURL = preferences.getString("server_url", "");  // Загружаем URL

  if (ssid.length() > 0 && password.length() > 0 && serverURL.length() > 0) {
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

String WiFiManager::getServerURL() {
  return serverURL;
}

bool WiFiManager::checkForSetupMode() {
  if (userInput.confirmWithTimeout(10000)) {  // 10 секунд таймаут
    needsSetup = true;
    Serial.println("Entering setup mode...");
    return true;
  }
  needsSetup = false;
  Serial.println("Starting with saved settings");
  return false;
}


bool WiFiManager::requiresSetup() {
  return needsSetup;
}


