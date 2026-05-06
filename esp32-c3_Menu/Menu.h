#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

class Menu {
private:
  int menuChoice;
  bool ledState;
  int ledPin;
  void printMenu();
  void handleMenuChoice(int choice); 
  void SerialBuferClear(); 

public:
    Menu(int pin);
    void begin();
    void update();
};

#endif
