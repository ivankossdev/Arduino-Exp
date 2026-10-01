// ============================================================
// WiFiManager — реализация для ESP32 (Core 3.x)
// ============================================================
// ИЗМЕНЕНО: WiFi.setAutoConnect() удалён в ESP32 Core 3.x — строка убрана
// ИЗМЕНЕНО: ENC_TYPE_* → WIFI_AUTH_* (ESP32 использует другие константы)
// ============================================================

#include "WiFiManager.h"

int WiFiManager::scan(NetworkInfo* results, int maxCount) {
  if (results == nullptr || maxCount <= 0) return -1;

  int n = WiFi.scanNetworks(false, true);

  if (n <= 0) {
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
  // ИЗМЕНЕНО: WiFi.setAutoConnect(false) удалён в ESP32 Core 3.x
  // На ESP32 достаточно setAutoReconnect — автоподключение и так отключено
  // после disconnect(). Строка просто удалена.
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
      Serial.printf("[WiFi] FAIL (status=%d)\n", WiFi.status());
      return false;
    }
  }
  Serial.printf("[WiFi] OK, IP %s\n", WiFi.localIP().toString().c_str());
  return true;
}

const char* WiFiManager::getEncryptionType(uint8_t encType) const {
  // ИЗМЕНЕНО: на ESP32 используются константы WIFI_AUTH_* вместо ENC_TYPE_*
  switch (encType) {
    case WIFI_AUTH_OPEN:            return "OPEN";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-ENT";
    default:                        return "UNKN";
  }
}