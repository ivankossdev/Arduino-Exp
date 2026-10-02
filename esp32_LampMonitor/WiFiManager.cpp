// ============================================================
// WiFiManager — реализация для ESP32 (Core 3.x)
// ============================================================
// ИЗМЕНЕНО: все Serial.print* заменены на LOG/LOG_LN с категорией
//           LOG_CAT_WIFI. Логи Wi-Fi теперь можно отключить одной
//           строкой в config.h.
// ============================================================

#include "WiFiManager.h"
#include "config.h"
#include "log.h"       // ИЗМЕНЕНО: подключены макросы логирования

int WiFiManager::scan(NetworkInfo* results, int maxCount) {
  if (results == nullptr || maxCount <= 0) return -1;

  int n = WiFi.scanNetworks(false, true);

  if (n <= 0) {
    LOG(LOG_CAT_WIFI, "[WiFi] Скан: %d сетей\n", n);
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
  LOG(LOG_CAT_WIFI, "[WiFi] Скан: %d сетей\n", count);
  return count;
}

bool WiFiManager::connectToNetwork(const char* ssid, const char* password) {
  if (ssid == nullptr || strlen(ssid) == 0) return false;

  LOG(LOG_CAT_WIFI, "[WiFi] Подключение к \"%s\"...\n", ssid);

  WiFiMode_t prevMode = WiFi.getMode();
  bool apWasActive = (prevMode == WIFI_AP || prevMode == WIFI_AP_STA);

  WiFi.persistent(false);
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(true, true);
  delay(WIFI_SETTLE_DELAY_MS);

  if (apWasActive) {
    WiFi.mode(WIFI_AP_STA);
  } else {
    WiFi.mode(WIFI_STA);
  }
  delay(WIFI_SETTLE_DELAY_MS);

  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_SETTLE_DELAY_MS);
    if (millis() - start > WIFI_CONNECT_TIMEOUT_MS) {
      LOG(LOG_CAT_WIFI, "[WiFi] FAIL (status=%d)\n", WiFi.status());
      return false;
    }
  }
  LOG(LOG_CAT_WIFI, "[WiFi] OK, IP %s\n", WiFi.localIP().toString().c_str());
  return true;
}

const char* WiFiManager::getEncryptionType(uint8_t encType) const {
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