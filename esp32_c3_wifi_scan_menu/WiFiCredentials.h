#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H

#include <Arduino.h>
#include <Preferences.h>

class WiFiCredentials {
private:
  static const char* NAMESPACE;
  static const char* KEY;

  // Вспомогательные методы
  String loadData() const;
  void saveData(const String& data);
  int findSSID(const String& ssid, const String& data) const;

public:
  WiFiCredentials();
  ~WiFiCredentials();

  bool save(const String& ssid, const String& password);
  String getPassword(const String& ssid) const;
  bool remove(const String& ssid);
  void clearAll();
  void printAll() const;
  int count() const;
  String getSSID(int index) const;
  String getPasswordByIndex(int index) const;
  bool hasCredentials(const String& ssid) const;
};

#endif