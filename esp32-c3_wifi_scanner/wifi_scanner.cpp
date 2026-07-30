#include "wifi_scanner.h"

void WiFiScanner::scan() {
  Serial.println(F("🔍 Начинаем сканирование Wi-Fi..."));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  _networkCount = WiFi.scanNetworks(false, true);

  if (_networkCount == 0) {
    Serial.println(F("❌ Нет доступных сетей."));
    WiFi.scanDelete();
    return;
  }

  if (_networkCount < 0) {
    Serial.printf("❌ Ошибка сканирования Wi-Fi: код %d\n", _networkCount);
    WiFi.scanDelete();
    return;
  }

  // ------ Подготовка данных ------
  const int n = _networkCount;
  String ssids[n];
  String rssiStr[n];
  String encStr[n];
  String hiddenStr[n];
  String bssidStr[n];
  String numStr[n];

  // Заголовки (тоже участвуют в вычислении ширины)
  const char* headers[] = { "№", "Канал", "RSSI", "Шифр", "Скрыта", "BSSID", "SSID" };
  // Сохраним отдельно значения для канала и номер, чтобы позже вывести числа
  int channels[n];

  // Максимальные длины (инициализируем длинами заголовков)
  int maxLen[7] = {
    strlen(headers[0]),
    strlen(headers[1]),
    strlen(headers[2]),
    strlen(headers[3]),
    strlen(headers[4]),
    strlen(headers[5]),
    strlen(headers[6])
  };

  // Заполняем массивы и обновляем максимумы
  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    int32_t rssi = WiFi.RSSI(i);
    uint8_t encType = WiFi.encryptionType(i);
    uint8_t* bssid = WiFi.BSSID(i);
    int32_t channel = WiFi.channel(i);
    bool hidden = (ssid.length() == 0);

    // Номер
    numStr[i] = String(i);
    if (numStr[i].length() > maxLen[0]) maxLen[0] = numStr[i].length();

    // Канал (число)
    channels[i] = channel;
    String chStr = String(channel);
    if (chStr.length() > maxLen[1]) maxLen[1] = chStr.length();

    // RSSI
    rssiStr[i] = String(rssi);
    if (rssiStr[i].length() > maxLen[2]) maxLen[2] = rssiStr[i].length();

    // Шифрование
    const char* enc = getEncryptionType(encType);
    encStr[i] = String(enc);
    if (encStr[i].length() > maxLen[3]) maxLen[3] = encStr[i].length();

    // Скрыта
    hiddenStr[i] = hidden ? "Да" : "Нет";
    if (hiddenStr[i].length() > maxLen[4]) maxLen[4] = hiddenStr[i].length();

    // BSSID
    char bssidBuff[18];
    snprintf(bssidBuff, sizeof(bssidBuff), "%02X:%02X:%02X:%02X:%02X:%02X",
             bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    bssidStr[i] = String(bssidBuff);
    if (bssidStr[i].length() > maxLen[5]) maxLen[5] = bssidStr[i].length();

    // SSID (если скрыта, то покажем "<скрытая сеть>")
    if (hidden) {
      ssids[i] = "<скрытая сеть>";
    } else {
      ssids[i] = ssid;
    }
    if (ssids[i].length() > maxLen[6]) maxLen[6] = ssids[i].length();
  }

  // Добавим небольшие отступы между колонками (по одному пробелу)
  int padding = 1;
  for (int i = 0; i < 7; i++) {
    maxLen[i] += padding;
  }

  // ------ Вывод таблицы ------
  Serial.printf("✅ Найдено %d сетей:\n\n", n);

  // Вывод заголовков
  Serial.printf("%-*s| %-*s| %-*s| %-*s| %-*s| %-*s| %-*s\n",
                maxLen[0], headers[0],
                maxLen[1], headers[1],
                maxLen[2], headers[2],
                maxLen[3], headers[3],
                maxLen[4], headers[4],
                maxLen[5], headers[5],
                maxLen[6], headers[6]);

  // Разделительная линия (повторим символы '-' для каждой колонки)
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < maxLen[i]; j++) Serial.print('-');
    if (i < 6) Serial.print('+');
  }
  Serial.println();

  // Вывод строк с данными
  for (int i = 0; i < n; i++) {
    Serial.printf("%-*s| %-*s| %-*s| %-*s| %-*s| %-*s| %-*s\n",
                  maxLen[0], numStr[i].c_str(),
                  maxLen[1], String(channels[i]).c_str(),
                  maxLen[2], rssiStr[i].c_str(),
                  maxLen[3], encStr[i].c_str(),
                  maxLen[4], hiddenStr[i].c_str(),
                  maxLen[5], bssidStr[i].c_str(),
                  maxLen[6], ssids[i].c_str());
  }
  Serial.println();

  WiFi.scanDelete();
}

// ----- Реализация getEncryptionType (как и раньше) -----
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