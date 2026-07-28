#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

struct WifiConfig {
  String ssid;
  String password;
};

struct MqttConfig {
  bool enabled = false;
  String broker;
  int port = 1883;
  String user;
  String password;
  String cmdTopic;
  String stateTopic;
};

struct DeviceConfig {
  WifiConfig wifi;
  int ledPin = 8;
  int defaultTemp = 21;
  int tempMax = 30;
  int tempMin = 18;
  MqttConfig mqtt;
};

class ConfigManager {
public:
  bool load();
  DeviceConfig getConfig() const { return _config; }

private:
  DeviceConfig _config;
};

#endif
