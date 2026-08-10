#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H

#include <Arduino.h>
#include <Preferences.h>
#include <ArduinoJson.h>

class WiFiCredentials {
private:
  static const char* NAMESPACE;
  static const char* KEY;

  DynamicJsonDocument* _doc; // указатель для динамического выделения
  bool _loaded;

  void loadData();
  void saveData();
  void ensureLoaded();
  String serializeData() const;

public:
  WiFiCredentials();
  ~WiFiCredentials();

  bool save(const String& ssid, const String& password);
  String getPassword(const String& ssid);
  bool remove(const String& ssid);
  void clearAll();
  void printAll();
  int count();
  String getSSID(int index);
  String getPasswordByIndex(int index);
  bool hasCredentials(const String& ssid);
};

#endif