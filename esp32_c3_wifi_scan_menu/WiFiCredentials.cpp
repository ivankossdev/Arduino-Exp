#include "WiFiCredentials.h"

const char* WiFiCredentials::NAMESPACE = "wifi";
const char* WiFiCredentials::KEY = "creds";

WiFiCredentials::WiFiCredentials() {
  // Ничего не делаем, все операции будут открывать Preferences локально
}

WiFiCredentials::~WiFiCredentials() {
  // Ничего не делаем
}

String WiFiCredentials::loadData() const {
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

int WiFiCredentials::findSSID(const String& ssid, const String& data) const {
  int pos = 0;
  int idx = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    String currentSSID = data.substring(pos, colon);
    if (currentSSID == ssid) {
      return idx;
    }
    pos = semi + 1;
    idx++;
  }
  return -1;
}

bool WiFiCredentials::save(const String& ssid, const String& password) {
  if (ssid.length() == 0 || password.length() == 0) return false;

  String data = loadData();

  // Проверяем, существует ли уже запись
  int index = findSSID(ssid, data);
  if (index != -1) {
    // Удаляем старую запись из строки
    int pos = 0;
    while (pos < data.length()) {
      int colon = data.indexOf(':', pos);
      if (colon == -1) break;
      int semi = data.indexOf(';', colon);
      if (semi == -1) semi = data.length();
      String currentSSID = data.substring(pos, colon);
      if (currentSSID == ssid) {
        data.remove(pos, semi - pos + 1);
        break;
      }
      pos = semi + 1;
    }
  }

  // Добавляем новую запись
  data += ssid + ":" + password + ";";
  saveData(data);
  return true;
}

String WiFiCredentials::getPassword(const String& ssid) const {
  String data = loadData();
  int pos = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    String currentSSID = data.substring(pos, colon);
    if (currentSSID == ssid) {
      return data.substring(colon + 1, semi);
    }
    pos = semi + 1;
  }
  return "";
}

bool WiFiCredentials::remove(const String& ssid) {
  String data = loadData();
  int pos = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    String currentSSID = data.substring(pos, colon);
    if (currentSSID == ssid) {
      data.remove(pos, semi - pos + 1);
      saveData(data);
      return true;
    }
    pos = semi + 1;
  }
  return false;
}

void WiFiCredentials::clearAll() {
  saveData("");
}

void WiFiCredentials::printAll() const {
  String data = loadData();
  if (data.length() == 0) {
    Serial.println("Нет сохранённых сетей.");
    return;
  }
  Serial.println("Сохранённые сети (SSID:пароль):");
  int pos = 0;
  int idx = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    String ssid = data.substring(pos, colon);
    String pass = data.substring(colon + 1, semi);
    Serial.printf("%d. %s : %s\n", idx, ssid.c_str(), pass.c_str());
    pos = semi + 1;
    idx++;
  }
}

int WiFiCredentials::count() const {
  String data = loadData();
  int cnt = 0;
  int pos = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    cnt++;
    pos = semi + 1;
  }
  return cnt;
}

String WiFiCredentials::getSSID(int index) const {
  String data = loadData();
  int pos = 0;
  int idx = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    if (idx == index) {
      return data.substring(pos, colon);
    }
    pos = semi + 1;
    idx++;
  }
  return "";
}

String WiFiCredentials::getPasswordByIndex(int index) const {
  String data = loadData();
  int pos = 0;
  int idx = 0;
  while (pos < data.length()) {
    int colon = data.indexOf(':', pos);
    if (colon == -1) break;
    int semi = data.indexOf(';', colon);
    if (semi == -1) semi = data.length();
    if (idx == index) {
      return data.substring(colon + 1, semi);
    }
    pos = semi + 1;
    idx++;
  }
  return "";
}

bool WiFiCredentials::hasCredentials(const String& ssid) const {
  String data = loadData();
  return findSSID(ssid, data) != -1;
}