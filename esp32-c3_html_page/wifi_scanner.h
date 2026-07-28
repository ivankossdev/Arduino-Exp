#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H
#include <WiFi.h>
#include <Arduino.h>

class WiFiScanner {
public:
  // Запускает сканирование и выводит результат в Serial
  void scan();

  // Возвращает количество найденных сетей
  int getNetworkCount() const { return _networkCount; }

private:
  int _networkCount = 0;

  // Вспомогательная функция: определяет тип шифрования
  const char* getEncryptionType(uint8_t encType);
};

#endif
