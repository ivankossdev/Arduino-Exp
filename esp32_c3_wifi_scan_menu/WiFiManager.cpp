#include "WiFiManager.h"

int WiFiManager::scan(NetworkInfo* results, int maxCount) {
  if (results == nullptr || maxCount <= 0) {
    return -1;
  }

  Serial.println(F("🔍 Сканирование Wi-Fi..."));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

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

/* НОВЫЙ МЕТОД: подключение к сети */
bool WiFiManager::connectToNetwork(const char* ssid, const char* password) {
  if (ssid == nullptr || strlen(ssid) == 0) {
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Начинаем подключение
  WiFi.begin(ssid, password);

  // Ждём до 10 секунд
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start > 10000) {
      Serial.println("\n⏱️ Таймаут подключения.");
      return false;
    }
  }
  Serial.println();
  return WiFi.status() == WL_CONNECTED;
}

const char* WiFiManager::getEncryptionType(uint8_t encType) const {
  switch (encType) {
    case WIFI_AUTH_OPEN:               return "OPEN";
    case WIFI_AUTH_WEP:                return "WEP";
    case WIFI_AUTH_WPA_PSK:            return "WPA";
    case WIFI_AUTH_WPA2_PSK:           return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:       return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE:    return "WPA2-ENT";
    case WIFI_AUTH_WPA3_PSK:           return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:      return "WPA2/WPA3";
    case WIFI_AUTH_WAPI_PSK:           return "WAPI";
    default:                           return "UNKN";
  }
}