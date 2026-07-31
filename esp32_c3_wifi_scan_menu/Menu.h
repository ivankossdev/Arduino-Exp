#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "wifi_scanner.h"


class Menu {
private:
  int menuChoice;
  void printMenu();
  void handleMenuChoice(int choice);
  void SerialBuferClear();
  void wifiScan();
  WiFiScanner scanner; 

public:
  Menu();
  void begin();
  void update();
};

#endif
