#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <Preferences.h>

class Menu {
private:
  int menuChoice;
  bool ledState;
  int ledPin;
  void printMenu();
  void handleMenuChoice(int choice);
  void SerialBuferClear();
  void saveSettings();
  void loadSettings();
  Preferences preferences;

public:
  Menu(int pin);
  void begin();
  void update();
};

#endif
