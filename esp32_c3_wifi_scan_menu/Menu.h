#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "wifi_scanner.h"

class Menu {
private:
  static const int MAX_NETWORKS = 50;       // Максимальное число сетей для отображения
  int _menuChoice;
  bool _scanning;                           // Флаг: идёт ли сканирование
  WiFiScanner _scanner;
  NetworkInfo _networks[MAX_NETWORKS];      // Буфер для результатов сканирования

  void printMenu();
  void handleMenuChoice(int choice);
  void SerialBufferClear();
  void wifiScan();
  void displayNetworks(const NetworkInfo* networks, int count); // Вывод таблицы

public:
  Menu();
  void begin();
  void update();
};

#endif