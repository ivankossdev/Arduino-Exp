#include "LedManager.h"

LedManager::LedManager(int pin){
  _pin = pin;
  pinMode(_pin, OUTPUT);
}


void LedManager::On(){
  digitalWrite(_pin, HIGH); 
}

 
void LedManager::Off(){
  digitalWrite(_pin, LOW);
}