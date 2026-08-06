#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H

#include <Arduino.h>
#include <Preferences.h>
#include <ArduinoJson.h>

class WiFiCredentials {
private:
  static const char* NAMESPACE;
  static const char* KEY;

  // Вспомогательные методы (теперь не const, т.к. могут вызывать saveData при миграции)
  String loadData();
  void saveData(const String& data);
  JsonDocument parseData(const String& data);
  String serializeData(const JsonDocument& doc) const;  // можно const
  static String migrateFromOldFormat(const String& data); // статический

public:
  WiFiCredentials();
  ~WiFiCredentials();

  bool save(const String& ssid, const String& password);
  String getPassword(const String& ssid);          // убрали const
  bool remove(const String& ssid);
  void clearAll();
  void printAll();                                 // убрали const
  int count();                                     // убрали const
  String getSSID(int index);                       // убрали const
  String getPasswordByIndex(int index);            // убрали const
  bool hasCredentials(const String& ssid);         // убрали const
};

#endif