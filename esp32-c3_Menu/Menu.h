#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <Preferences.h>
#include "LedManager.h"


class Menu {
private:
  int menuChoice;
  int ledPin;
  void printMenu();
  void handleMenuChoice(int choice);
  void SerialBuferClear();
  void saveSettings();
  void loadSettings();
  void applyLedState();
  void printFlashInfo();
  void printSystemStatus();
  Preferences preferences;
  LedManager led; 

public:
  Menu(int pin);
  void begin();
  void update();
};

#endif
