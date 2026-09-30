#include "WiFiManager.h"

int WiFiManager::scan(NetworkInfo* results, int maxCount) {
  if (results == nullptr || maxCount <= 0) return -1;

  Serial.println(F("🔍 Сканирование Wi-Fi..."));

  // На ESP8266 оставляем режим как есть — в AP-режиме используется AP+STA,
  // чтобы клиент не отваливался на время скана.
  int n = WiFi.scanNetworks(false, true);

  if (n == 0) {
    Serial.println(F("❌ Сетей не найдено."));
    WiFi.scanDelete();
    return 0;
  }

  if (n < 0) {
    Serial.printf("❌ Ошибка сканирования: %d\n", n);
    WiFi.scanDelete();
    return n;
  }

  int count = (n < maxCount) ? n : maxCount;

  for (int i = 0; i < count; i++) {
    NetworkInfo* info = &results[i];

    String ssid = WiFi.SSID(i);
    if (ssid.length() == 0) {
      info->hidden = true;
      strncpy(info->ssid, "<hidden>", sizeof(info->ssid) - 1);
      info->ssid[sizeof(info->ssid) - 1] = '\0';
    } else {
      info->hidden = false;
      strncpy(info->ssid, ssid.c_str(), sizeof(info->ssid) - 1);
      info->ssid[sizeof(info->ssid) - 1] = '\0';
    }

    uint8_t* bssid = WiFi.BSSID(i);
    snprintf(info->bssid, sizeof(info->bssid), "%02X:%02X:%02X:%02X:%02X:%02X",
             bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

    info->rssi = WiFi.RSSI(i);
    info->channel = WiFi.channel(i);
    info->encryptionType = WiFi.encryptionType(i);
  }

  WiFi.scanDelete();
  Serial.printf("✅ Найдено %d сетей (показано %d)\n", n, count);
  return count;
}

bool WiFiManager::connectToNetwork(const char* ssid, const char* password) {
  if (ssid == nullptr || strlen(ssid) == 0) return false;

  Serial.printf("🔄 Подключение к \"%s\"\n", ssid);

  // 1. Полная очистка сохранённых кредов и сброс радио
  WiFi.persistent(false);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(true, true);   // wifioff=true, eraseap=true — стирает SSID/пароль из flash
  delay(200);
  WiFi.mode(WIFI_OFF);
  delay(200);
  WiFi.mode(WIFI_STA);
  delay(200);

  // 2. Ищем целевую сеть в скане и запоминаем канал
  int targetChannel = 0;
  int n = WiFi.scanNetworks(false, true);
  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == String(ssid)) {
      targetChannel = WiFi.channel(i);
      Serial.printf("🎯 Найдена \"%s\" на канале %d\n", ssid, targetChannel);
      break;
    }
  }
  WiFi.scanDelete();

  if (targetChannel == 0) {
    Serial.println("⚠️ Сеть не найдена при сканировании — пробуем без указания канала");
  }

  // 3. Подключаемся (с каналом, если нашли)
  if (targetChannel > 0) {
    WiFi.begin(ssid, password, targetChannel);
  } else {
    WiFi.begin(ssid, password);
  }

  unsigned long start = millis();
  int lastStatus = -1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    int st = WiFi.status();
    if (st != lastStatus) {
      lastStatus = st;
      Serial.printf("  status → %d\n", st);
    }
    if (millis() - start > 20000) {
      Serial.printf("\n⏱️ Таймаут. Статус: %d\n", st);
      WiFi.printDiag(Serial);
      return false;
    }
  }
  Serial.printf("\n✅ Подключено: %s, IP: %s\n",
                ssid, WiFi.localIP().toString().c_str());
  return true;
}

// === ИЗМЕНЕНО: на ESP8266 константы шифрования другие ===
const char* WiFiManager::getEncryptionType(uint8_t encType) const {
  switch (encType) {
    case ENC_TYPE_NONE: return "OPEN";
    case ENC_TYPE_WEP: return "WEP";
    case ENC_TYPE_TKIP: return "WPA";
    case ENC_TYPE_CCMP: return "WPA2";
    case ENC_TYPE_AUTO: return "AUTO";
    default: return "UNKN";
  }
}