#ifndef MQTT_CREDENTIALS_H
#define MQTT_CREDENTIALS_H

#include <Arduino.h>
#include <Preferences.h>
#include <ArduinoJson.h>

class MqttCredentials {
private:
  static const char* NAMESPACE;
  static const char* KEY;

  String loadData();
  void saveData(const String& data);
  JsonDocument parseData(const String& data);
  String serializeData(const JsonDocument& doc) const;

public:
  MqttCredentials();
  ~MqttCredentials();

  bool load();
  bool save();

  String getServer() const;
  void setServer(const String& server);
  int getPort() const;
  void setPort(int port);
  String getUser() const;
  void setUser(const String& user);
  String getPassword() const;
  void setPassword(const String& password);

  // === УДАЛЕНО: cmdTopic ===
  // === ОСТАВЛЕНО: stateTopic — единственный топик, который слушаем ===
  String getStateTopic() const;
  void setStateTopic(const String& stateTopic);

  bool isEmpty() const;

private:
  String _server;
  int _port;
  String _user;
  String _password;
  String _stateTopic;   // === УДАЛЕНО: _cmdTopic ===
};

#endif