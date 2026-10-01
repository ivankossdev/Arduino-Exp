#include "WiFiCredentials.h"

const char* WiFiCredentials::NAMESPACE = "wifi";
const char* WiFiCredentials::KEY = "creds";

WiFiCredentials::WiFiCredentials()
    : _doc(new DynamicJsonDocument(2048)), _loaded(false) {}

WiFiCredentials::~WiFiCredentials() {
  delete _doc;
}

void WiFiCredentials::loadData() {
  Preferences prefs;
  prefs.begin(NAMESPACE, false);
  String data = prefs.getString(KEY, "");
  prefs.end();

  if (data.length() == 0) {
    _doc->clear();
    return;
  }

  if (data[0] == '{') {
    DeserializationError error = deserializeJson(*_doc, data);
    if (error) {
      Serial.println("Ошибка парсинга JSON, данные сброшены.");
      _doc->clear();
    }
    return;
  }

  // Миграция старого формата
  Serial.println("Обнаружен старый формат хранения, выполняю миграцию...");
  DynamicJsonDocument newDoc(1024);
  int pos = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    String ssid = data.substring(pos, colon);
    String pass = data.substring(colon + 1, semi);
    if (ssid.length() > 0 && pass.length() > 0) {
      newDoc[ssid] = pass;
    }
    pos = semi + 1;
  }
  _doc->clear();
  for (JsonPair kv : newDoc.as<JsonObject>()) {
    (*_doc)[kv.key().c_str()] = kv.value().as<String>();
  }
  saveData();
  Serial.println("Миграция завершена успешно.");
}

void WiFiCredentials::saveData() {
  Preferences prefs;
  prefs.begin(NAMESPACE, false);
  String output;
  serializeJson(*_doc, output);
  prefs.putString(KEY, output);
  prefs.end();
}

void WiFiCredentials::ensureLoaded() {
  if (!_loaded) {
    loadData();
    _loaded = true;
  }
}

String WiFiCredentials::serializeData() const {
  String output;
  serializeJson(*_doc, output);
  return output;
}

bool WiFiCredentials::save(const String& ssid, const String& password) {
  if (ssid.length() == 0 || password.length() == 0) return false;
  ensureLoaded();
  (*_doc)[ssid] = password;
  saveData();
  return true;
}

String WiFiCredentials::getPassword(const String& ssid) {
  ensureLoaded();
  if (_doc->containsKey(ssid)) {
    return (*_doc)[ssid].as<String>();
  }
  return "";
}

bool WiFiCredentials::remove(const String& ssid) {
  ensureLoaded();
  if (!_doc->containsKey(ssid)) return false;
  _doc->remove(ssid);
  saveData();
  return true;
}

void WiFiCredentials::clearAll() {
  _doc->clear();
  saveData();
  _loaded = true;
}

void WiFiCredentials::printAll() {
  ensureLoaded();
  if (_doc->size() == 0) {
    Serial.println("Нет сохранённых сетей.");
    return;
  }
  Serial.println("Сохранённые сети (SSID):");
  int idx = 0;
  for (JsonPair kv : _doc->as<JsonObject>()) {
    Serial.printf("%d. %s\n", idx, kv.key().c_str());
    idx++;
  }
}

int WiFiCredentials::count() {
  ensureLoaded();
  return _doc->size();
}

String WiFiCredentials::getSSID(int index) {
  ensureLoaded();
  int idx = 0;
  for (JsonPair kv : _doc->as<JsonObject>()) {
    if (idx == index) {
      return kv.key().c_str();
    }
    idx++;
  }
  return "";
}

String WiFiCredentials::getPasswordByIndex(int index) {
  ensureLoaded();
  int idx = 0;
  for (JsonPair kv : _doc->as<JsonObject>()) {
    if (idx == index) {
      return kv.value().as<String>();
    }
    idx++;
  }
  return "";
}

bool WiFiCredentials::hasCredentials(const String& ssid) {
  ensureLoaded();
  return _doc->containsKey(ssid);
}