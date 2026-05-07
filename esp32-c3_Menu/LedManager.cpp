#include "LedManager.h"


void LedManager::begin(int pin){
  _pin = pin;
  _state = false; 
  pinMode(_pin, OUTPUT);
}


void LedManager::on(){
  setState(true);
  digitalWrite(_pin, LOW); 
}

 
void LedManager::off(){
  setState(true);
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