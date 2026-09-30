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

  // === ИЗМЕНЕНО: не отключаем радио полностью, только рвём текущую сессию ===
  WiFi.disconnect(false);
  delay(100);

  // === ИЗМЕНЕНО: переключаем режим явно и даём радио просыпаться ===
  WiFi.mode(WIFI_STA);
  delay(200);

  WiFi.begin(ssid, password);

  unsigned long start = millis();
  unsigned long lastLog = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    // === ДОБАВЛЕНО: раз в секунду печатаем код статуса ===
    //  0=IDLE, 1=NO_SSID, 2=SCAN_DONE, 3=CONNECTED,
    //  4=CONNECT_FAILED, 5=CONNECTION_LOST, 6=DISCONNECTED
    if (millis() - lastLog > 1000) {
      lastLog = millis();
      Serial.printf("  [%lus] WiFi.status()=%d\n",
                    (millis() - start) / 1000, WiFi.status());
    }
    if (millis() - start > 15000) {   // === ИЗМЕНЕНО: 15 сек вместо 10 ===
      Serial.printf("\n⏱️ Таймаут. Итоговый статус: %d\n", WiFi.status());
      WiFi.disconnect(true);
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
    case ENC_TYPE_NONE:  return "OPEN";
    case ENC_TYPE_WEP:   return "WEP";
    case ENC_TYPE_TKIP:  return "WPA";
    case ENC_TYPE_CCMP:  return "WPA2";
    case ENC_TYPE_AUTO:  return "AUTO";
    default:             return "UNKN";
  }
}