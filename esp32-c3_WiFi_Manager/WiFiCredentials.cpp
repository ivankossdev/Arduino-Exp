#include "WiFiCredentials.h"

const char* WiFiCredentials::NAMESPACE = "wifi";
const char* WiFiCredentials::KEY = "creds";

WiFiCredentials::WiFiCredentials() {}
WiFiCredentials::~WiFiCredentials() {}

// ---------- Базовые операции ----------
String WiFiCredentials::loadData() {   // больше не const
  Preferences prefs;
  prefs.begin(NAMESPACE, false);
  String data = prefs.getString(KEY, "");
  prefs.end();
  return data;
}

void WiFiCredentials::saveData(const String& data) {
  Preferences prefs;
  prefs.begin(NAMESPACE, false);
  prefs.putString(KEY, data);
  prefs.end();
}

// ---------- Вспомогательные методы ----------
JsonDocument WiFiCredentials::parseData(const String& data) { // больше не const
  JsonDocument doc;

  if (data.length() == 0) {
    return doc;
  }

  // Проверяем, является ли строка JSON
  if (data[0] == '{') {
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      Serial.println("Ошибка парсинга JSON, данные будут сброшены.");
      doc.clear();
    }
    return doc;
  }

  // --- Миграция старого формата ---
  Serial.println("Обнаружен старый формат хранения, выполняю миграцию...");
  String migrated = migrateFromOldFormat(data);
  if (migrated.length() > 0) {
    DeserializationError error = deserializeJson(doc, migrated);
    if (!error) {
      saveData(migrated);  // теперь можно, т.к. метод не const
      Serial.println("Миграция завершена успешно.");
      return doc;
    }
  }
  Serial.println("Ошибка миграции, данные сброшены.");
  doc.clear();
  return doc;
}

String WiFiCredentials::serializeData(const JsonDocument& doc) const {
  String output;
  serializeJson(doc, output);
  return output;
}

// статический метод
String WiFiCredentials::migrateFromOldFormat(const String& data) {
  JsonDocument doc;
  int pos = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    String ssid = data.substring(pos, colon);
    String pass = data.substring(colon + 1, semi);
    if (ssid.length() > 0 && pass.length() > 0) {
      doc[ssid] = pass;
    }
    pos = semi + 1;
  }
  String output;
  serializeJson(doc, output);
  return output;
}

// ---------- Публичные методы ----------
bool WiFiCredentials::save(const String& ssid, const String& password) {
  if (ssid.length() == 0 || password.length() == 0) return false;

  String data = loadData();
  JsonDocument doc = parseData(data);
  doc[ssid] = password;
  String newData = serializeData(doc);
  saveData(newData);
  return true;
}

String WiFiCredentials::getPassword(const String& ssid) {
  String data = loadData();
  JsonDocument doc = parseData(data);
  if (doc.containsKey(ssid)) {
    return doc[ssid].as<String>();
  }
  return "";
}

bool WiFiCredentials::remove(const String& ssid) {
  String data = loadData();
  JsonDocument doc = parseData(data);
  if (!doc.containsKey(ssid)) return false;

  doc.remove(ssid);
  String newData = serializeData(doc);
  saveData(newData);
  return true;
}

void WiFiCredentials::clearAll() {
  saveData("{}");
}

void WiFiCredentials::printAll() {
  String data = loadData();
  JsonDocument doc = parseData(data);
  if (doc.size() == 0) {
    Serial.println("Нет сохранённых сетей.");
    return;
  }
  Serial.println("Сохранённые сети (SSID):");
  int idx = 0;
  for (JsonPair kv : doc.as<JsonObject>()) {
    Serial.printf("%d. %s\n", idx, kv.key().c_str());
    idx++;
  }
}

int WiFiCredentials::count() {
  String data = loadData();
  JsonDocument doc = parseData(data);
  return doc.size();
}

String WiFiCredentials::getSSID(int index) {
  String data = loadData();
  JsonDocument doc = parseData(data);
  int idx = 0;
  for (JsonPair kv : doc.as<JsonObject>()) {
    if (idx == index) {
      return kv.key().c_str();
    }
    idx++;
  }
  return "";
}

String WiFiCredentials::getPasswordByIndex(int index) {
  String data = loadData();
  JsonDocument doc = parseData(data);
  int idx = 0;
  for (JsonPair kv : doc.as<JsonObject>()) {
    if (idx == index) {
      return kv.value().as<String>();
    }
    idx++;
  }
  return "";
}

bool WiFiCredentials::hasCredentials(const String& ssid) {
  String data = loadData();
  JsonDocument doc = parseData(data);
  return doc.containsKey(ssid);
}