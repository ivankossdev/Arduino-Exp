#include "WiFiManager.h"

int WiFiManager::scan(NetworkInfo* results, int maxCount) {
  if (results == nullptr || maxCount <= 0) return -1;

  Serial.println(F("🔍 Сканирование Wi-Fi..."));

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

// === ИЗМЕНЕНО: сохраняем AP, если он был активен ===
// Раньше здесь был WiFi.mode(WIFI_OFF), который гасил AP на время попытки
// подключения — пользователь, зашедший через веб в AP-режиме, терял связь
// с устройством на все 20 секунд ожидания.
//
// Теперь: если AP был активен, возвращаемся в WIFI_AP_STA — AP продолжает
// работать параллельно с попыткой STA-подключения. Если AP не был активен
// (обычный клиентский режим) — используется чистый WIFI_STA.
bool WiFiManager::connectToNetwork(const char* ssid, const char* password) {
  if (ssid == nullptr || strlen(ssid) == 0) return false;

  Serial.printf("🔄 Подключение к \"%s\"\n", ssid);

  // === ИСПРАВЛЕНО: на ESP8266 тип называется WiFiMode_t, не wifi_mode_t ===
  // (wifi_mode_t — это ESP-IDF/ESP32; в ядре ESP8266 Arduino используется
  //  WiFiMode_t. Константы WIFI_AP / WIFI_AP_STA тут же того же типа.)
  WiFiMode_t prevMode = WiFi.getMode();
  bool apWasActive = (prevMode == WIFI_AP || prevMode == WIFI_AP_STA);

  // 2. Сбрасываем креды и авто-подключение. disconnect(true, true):
  //    wifioff = true  — выключаем STA-радио
  //    eraseap = true  — стираем сохранённые SSID/пароль из flash
  //    ВАЖНО: softAP при этом не затрагивается.
  WiFi.persistent(false);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(true, true);
  delay(200);

  // 3. Возвращаемся в нужный режим
  if (apWasActive) {
    WiFi.mode(WIFI_AP_STA);   // AP остаётся, добавляется STA
  } else {
    WiFi.mode(WIFI_STA);      // чистый клиентский режим
  }
  delay(200);

  // 4. Подключаемся
  WiFi.begin(ssid, password);

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
      Serial.printf("\n⏱️ Таймаут. Итоговый статус: %d\n", st);
      WiFi.printDiag(Serial);
      return false;
    }
  }
  Serial.printf("\n✅ Подключено: %s, IP: %s\n",
                ssid, WiFi.localIP().toString().c_str());
  return true;
}

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