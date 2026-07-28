#include "wifi_scanner.h"


void WiFiScanner::scan() {
  Serial.println(F("🔍 Начинаем сканирование Wi-Fi..."));

  // Отключаемся и устанавливаем режим станции
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Запускаем синхронное сканирование (не асинхронное), включая скрытые сети
  _networkCount = WiFi.scanNetworks(false, true);

  if (_networkCount == 0) {
    Serial.println(F("❌ Нет доступных сетей."));
  } else if (_networkCount > 0) {
    Serial.printf(PSTR("✅ Найдено %d сетей:\n\n"), _networkCount);

    // Выводим таблицу
    Serial.println(F(" №  | Канал |    RSSI | Шифр | Скрыта |       BSSID       | SSID"));
    Serial.println(F("----+-------+---------+------+--------+-------------------+---------------------------"));

    for (int i = 0; i < _networkCount; i++) {
      String ssid = WiFi.SSID(i);
      int32_t rssi = WiFi.RSSI(i);
      uint8_t encType = WiFi.encryptionType(i);
      uint8_t* bssid = WiFi.BSSID(i);
      int32_t channel = WiFi.channel(i);
      bool hidden = (ssid.length() == 0);  // Если SSID пуст — сеть скрыта

      // Форматируем BSSID
      char bssidStr[18];
      snprintf(bssidStr, sizeof(bssidStr), "%02X:%02X:%02X:%02X:%02X:%02X",
               bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

      // Определяем тип шифрования
      const char* enc = getEncryptionType(encType);

      // Определяем, скрыта ли сеть
      const char* isHidden = hidden ? "Да" : "Нет";

      // Выводим строку таблицы
      Serial.printf("%2d | %5d | %7d | %4s | %6s | %s | %s\n",
                    i,
                    channel,
                    rssi,
                    enc,
                    isHidden,
                    bssidStr,
                    hidden ? "<скрытая сеть>" : ssid.c_str());
    }
    Serial.println();
  } else {
    Serial.printf(PSTR("❌ Ошибка сканирования Wi-Fi: код %d\n"), _networkCount);
  }

  // Очищаем результат сканирования, чтобы не мешать другим операциям
  WiFi.scanDelete();
}
