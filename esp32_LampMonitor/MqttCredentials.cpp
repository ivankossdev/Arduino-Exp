// ============================================================
// MqttCredentials — хранилище MQTT-настроек
// ============================================================
// ИЗМЕНЕНО: NAMESPACE и KEY берутся из config.h
// ИЗМЕНЕНО: дефолтный порт и state-топик берутся из config.h
// ============================================================

#include "MqttCredentials.h"
#include "config.h"      // ИЗМЕНЕНО: константы вынесены сюда

// ИЗМЕНЕНО: было "mqtt"
const char* MqttCredentials::NAMESPACE = PREF_NAMESPACE_MQTT;
// ИЗМЕНЕНО: было "config"
const char* MqttCredentials::KEY = PREF_KEY_MQTT;

// ИЗМЕНЕНО: было _port(1883), _stateTopic("home/lamp/status")
MqttCredentials::MqttCredentials()
    : _port(MQTT_DEFAULT_PORT), _stateTopic(MQTT_DEFAULT_STATE_TOPIC) {}

MqttCredentials::~MqttCredentials() {}

String MqttCredentials::loadData() {
  Preferences prefs;
  prefs.begin(NAMESPACE, false);
  String data = prefs.getString(KEY, "");
  prefs.end();
  return data;
}

void MqttCredentials::saveData(const String& data) {
  Preferences prefs;
  prefs.begin(NAMESPACE, false);
  prefs.putString(KEY, data);
  prefs.end();
}

JsonDocument MqttCredentials::parseData(const String& data) {
  JsonDocument doc;
  if (data.length() == 0) {
    return doc;
  }
  DeserializationError error = deserializeJson(doc, data);
  if (error) {
    Serial.println("Ошибка парсинга JSON MQTT, данные сброшены.");
    doc.clear();
  }
  return doc;
}

String MqttCredentials::serializeData(const JsonDocument& doc) const {
  String output;
  serializeJson(doc, output);
  return output;
}

bool MqttCredentials::load() {
  String data = loadData();
  JsonDocument doc = parseData(data);
  if (doc.isNull()) {
    return false;
  }
  _server = doc["server"].as<String>();
  _port = doc["port"].as<int>();
  _user = doc["user"].as<String>();
  _password = doc["password"].as<String>();
  _stateTopic = doc["stateTopic"].as<String>();
  if (_stateTopic.length() == 0) {
    // ИЗМЕНЕНО: было "home/lamp/status"
    _stateTopic = MQTT_DEFAULT_STATE_TOPIC;
  }
  return true;
}

bool MqttCredentials::save() {
  JsonDocument doc;
  doc["server"] = _server;
  doc["port"] = _port;
  doc["user"] = _user;
  doc["password"] = _password;
  doc["stateTopic"] = _stateTopic;
  String data = serializeData(doc);
  saveData(data);
  return true;
}

String MqttCredentials::getServer() const { return _server; }
void MqttCredentials::setServer(const String& server) { _server = server; }
int MqttCredentials::getPort() const { return _port; }
void MqttCredentials::setPort(int port) { _port = port; }
String MqttCredentials::getUser() const { return _user; }
void MqttCredentials::setUser(const String& user) { _user = user; }
String MqttCredentials::getPassword() const { return _password; }
void MqttCredentials::setPassword(const String& password) { _password = password; }

String MqttCredentials::getStateTopic() const { return _stateTopic; }
void MqttCredentials::setStateTopic(const String& stateTopic) { _stateTopic = stateTopic; }

bool MqttCredentials::isEmpty() const {
  return _server.length() == 0 && _user.length() == 0 && _password.length() == 0;
}