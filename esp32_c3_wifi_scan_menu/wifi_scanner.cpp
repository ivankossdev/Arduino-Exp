#include "wifi_scanner.h"

void WiFiScanner::scan() {
  Serial.println(F("🔍 Scanning Wi-Fi..."));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  _networkCount = WiFi.scanNetworks(false, true);

  if (_networkCount == 0) {
    Serial.println(F("❌ No networks found."));
    WiFi.scanDelete();
    return;
  }

  if (_networkCount < 0) {
    Serial.printf("❌ Scan error: %d\n", _networkCount);
    WiFi.scanDelete();
    return;
  }

  const int n = _networkCount;
  
  // ------ Собираем данные в массивы ------
  String numStr[n];
  String channelStr[n];
  String rssiStr[n];
  String encStr[n];
  String hiddenStr[n];
  String bssidStr[n];
  String ssidStr[n];

  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    int32_t rssi = WiFi.RSSI(i);
    uint8_t encType = WiFi.encryptionType(i);
    uint8_t* bssid = WiFi.BSSID(i);
    int32_t channel = WiFi.channel(i);
    bool hidden = (ssid.length() == 0);

    numStr[i] = String(i);
    channelStr[i] = String(channel);
    rssiStr[i] = String(rssi);
    encStr[i] = String(getEncryptionType(encType));
    hiddenStr[i] = hidden ? "Yes" : "No";

    char bssidBuff[18];
    snprintf(bssidBuff, sizeof(bssidBuff), "%02X:%02X:%02X:%02X:%02X:%02X",
             bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    bssidStr[i] = String(bssidBuff);

    ssidStr[i] = hidden ? "<hidden>" : ssid;
  }

  // ------ Заголовки (только ASCII) ------
  const char* headers[] = {"#", "CH", "RSSI", "ENC", "HID", "BSSID", "SSID"};
  const int colCount = 7;

  // ------ Вычисляем ширину колонок (максимум из заголовка и всех данных) ------
  int widths[colCount];
  for (int i = 0; i < colCount; i++) {
    widths[i] = strlen(headers[i]);        // начальная ширина = длина заголовка
  }

  for (int i = 0; i < n; i++) {
    if (numStr[i].length() > widths[0]) widths[0] = numStr[i].length();
    if (channelStr[i].length() > widths[1]) widths[1] = channelStr[i].length();
    if (rssiStr[i].length() > widths[2]) widths[2] = rssiStr[i].length();
    if (encStr[i].length() > widths[3]) widths[3] = encStr[i].length();
    if (hiddenStr[i].length() > widths[4]) widths[4] = hiddenStr[i].length();
    if (bssidStr[i].length() > widths[5]) widths[5] = bssidStr[i].length();
    if (ssidStr[i].length() > widths[6]) widths[6] = ssidStr[i].length();
  }

  // Добавляем по одному пробелу для отступа между колонками (увеличиваем ширину)
  for (int i = 0; i < colCount; i++) {
    widths[i] += 1;
  }

  // ------ Вывод таблицы ------
  Serial.printf("✅ Found %d networks:\n\n", n);

  // Заголовки
  for (int i = 0; i < colCount; i++) {
    Serial.printf("%-*s", widths[i], headers[i]);
    if (i < colCount - 1) Serial.print(" | ");
  }
  Serial.println();

  // Разделительная линия (дефисы + "-+-" между колонками)
  for (int i = 0; i < colCount; i++) {
    for (int j = 0; j < widths[i]; j++) Serial.print("-");
    if (i < colCount - 1) Serial.print("-+-");
  }
  Serial.println();

  // Данные
  for (int i = 0; i < n; i++) {
    Serial.printf("%-*s", widths[0], numStr[i].c_str());
    Serial.print(" | ");
    Serial.printf("%-*s", widths[1], channelStr[i].c_str());
    Serial.print(" | ");
    Serial.printf("%-*s", widths[2], rssiStr[i].c_str());
    Serial.print(" | ");
    Serial.printf("%-*s", widths[3], encStr[i].c_str());
    Serial.print(" | ");
    Serial.printf("%-*s", widths[4], hiddenStr[i].c_str());
    Serial.print(" | ");
    Serial.printf("%-*s", widths[5], bssidStr[i].c_str());
    Serial.print(" | ");
    Serial.printf("%-*s", widths[6], ssidStr[i].c_str());
    Serial.println();
  }
  Serial.println();

  WiFi.scanDelete();
}

// ----- Реализация getEncryptionType (без изменений) -----
const char* WiFiScanner::getEncryptionType(uint8_t encType) {
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