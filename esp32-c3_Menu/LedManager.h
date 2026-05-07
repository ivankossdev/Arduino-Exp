#ifndef LedManager_H
#define LedManager_H

#include <Arduino.h>

class LedManager {
public:
  bool begin(int pin);
  void on();
  void off();
  void blink();
  bool getState() const { return _state; }
  void setState(bool set); 
private:
  bool _state; 
  int _pin; 
};

#endif