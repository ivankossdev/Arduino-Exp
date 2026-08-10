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

  bool load(); // загружает в члены
  bool save(); // сохраняет из членов

  // Геттеры и сеттеры
  String getServer() const;
  void setServer(const String& server);
  int getPort() const;
  void setPort(int port);
  String getUser() const;
  void setUser(const String& user);
  String getPassword() const;
  void setPassword(const String& password);
  String getCmdTopic() const;
  void setCmdTopic(const String& cmdTopic);
  String getStateTopic() const;
  void setStateTopic(const String& stateTopic);

  bool isEmpty() const; // проверяет, есть ли какие-то данные

private:
  String _server;
  int _port;
  String _user;
  String _password;
  String _cmdTopic;
  String _stateTopic;
};

#endif