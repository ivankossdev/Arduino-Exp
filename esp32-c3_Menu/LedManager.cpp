#include "LedManager.h"


bool LedManager::begin(int pin) {
  if (pin < 0 || pin > 33) {
    return false;
  }
  _pin = pin;
  _state = false;
  pinMode(_pin, OUTPUT);
  return true;
}


void LedManager::on(){
  setState(true);
  digitalWrite(_pin, LOW); 
}

 
void LedManager::off(){
  setState(false);
  digitalWrite(_pin, HIGH);
}


void LedManager::blink(){
  on();
  delay(100);
  off();
  delay(100);
}


void LedManager::setState(bool set){
  _state = set; 
}