#ifndef LedManager_H
#define LedManager_H

#include <Arduino.h>

class LedManager {
public:
  void begin(int pin);
  void on();
  void off();
  void blink();
  bool state; 
private:
  int _pin; 
};

#endif