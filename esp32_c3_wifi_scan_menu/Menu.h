#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "WiFiManager.h"

class Menu {
private:
  static const int MAX_NETWORKS = 50;
  int _menuChoice;
  bool _scanning;
  bool _hasScanResult;                // флаг наличия результатов сканирования
  bool _terminalIsBusy;               // Теримнал занят, ожидаем ввода информации от пользователя
  WiFiManager _scanner;
  NetworkInfo _networks[MAX_NETWORKS];
  char buffer[30]; 

  void printMenu();
  void handleMenuChoice(int choice);
  void SerialBufferClear();
  void wifiScan();
  void displayNetworks(const NetworkInfo* networks, int count);
  void connectToNetwork();            // новый метод для подключения

public:
  Menu();
  void begin();
  void update();
};

#endif