#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H

#include <WiFi.h>
#include <Arduino.h>
#include <cstring>   // для strncpy

// Структура для хранения информации об одной сети
struct NetworkInfo {
  char ssid[33];            // максимум 32 символа + завершающий ноль
  char bssid[18];           // формат "XX:XX:XX:XX:XX:XX"
  int32_t rssi;
  uint8_t channel;
  uint8_t encryptionType;   // используем константы WIFI_AUTH_*
  bool hidden;
};

class WiFiScanner {
public:
  // Заполняет массив results (не более maxCount) и возвращает количество найденных сетей.
  // При ошибке возвращает отрицательное число.
  int scan(NetworkInfo* results, int maxCount);

  // Вспомогательная функция для получения строки типа шифрования (оставлена публичной для использования в Menu)
  const char* getEncryptionType(uint8_t encType) const;

private:
  int _networkCount = 0;    // не обязателен, но оставлен для возможного использования
};

#endif