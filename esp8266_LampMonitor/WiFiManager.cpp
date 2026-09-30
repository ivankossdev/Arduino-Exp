#include "WiFiManager.h"

int WiFiManager::scan(NetworkInfo* results, int maxCount) {
  if (results == nullptr || maxCount <= 0) return -1;

  int n = WiFi.scanNetworks(false, true);

  if (n <= 0) {
    // === ИЗМЕНЕНО: один короткий лог на скан, без эмодзи ===
    Serial.printf("[WiFi] Скан: %d сетей\n", n);
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
  Serial.printf("[WiFi] Скан: %d сетей\n", count);
  return count;
}

bool WiFiManager::connectToNetwork(const char* ssid, const char* password) {
  if (ssid == nullptr || strlen(ssid) == 0) return false;

  Serial.printf("[WiFi] Подключение к \"%s\"...\n", ssid);

  WiFiMode_t prevMode = WiFi.getMode();
  bool apWasActive = (prevMode == WIFI_AP || prevMode == WIFI_AP_STA);

  WiFi.persistent(false);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(true, true);
  delay(200);

  if (apWasActive) {
    WiFi.mode(WIFI_AP_STA);
  } else {
    WiFi.mode(WIFI_STA);
  }
  delay(200);

  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if (millis() - start > 20000) {
      // === ИЗМЕНЕНО: короткий лог на провал с кодом статуса ===
      Serial.printf("[WiFi] FAIL (status=%d)\n", WiFi.status());
      return false;
    }
  }
  // === ИЗМЕНЕНО: короткий лог на успех с IP ===
  Serial.printf("[WiFi] OK, IP %s\n", WiFi.localIP().toString().c_str());
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