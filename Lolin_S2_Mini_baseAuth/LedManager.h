#ifndef LedManager_H
#define LedManager_H

#include <Arduino.h>

class LedManager {
public:
  LedManager(int pin);
  void On();
  void Off();
private:
  int _pin; 
};

#endif