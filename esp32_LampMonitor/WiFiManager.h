// ============================================================
// WiFiManager — адаптация под ESP32
// ============================================================
// ИЗМЕНЕНО: ESP8266WiFi.h → WiFi.h
// ============================================================

#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>           // ИЗМЕНЕНО: было <ESP8266WiFi.h>
#include <Arduino.h>
#include <cstring>

struct NetworkInfo {
  char ssid[33];
  char bssid[18];
  int32_t rssi;
  uint8_t channel;
  uint8_t encryptionType;
  bool hidden;
};

class WiFiManager {
public:
  int scan(NetworkInfo* results, int maxCount);
  bool connectToNetwork(const char* ssid, const char* password);
  const char* getEncryptionType(uint8_t encType) const;

private:
  int _networkCount = 0;
};

#endif