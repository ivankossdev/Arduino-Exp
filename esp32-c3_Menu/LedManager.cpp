#include "LedManager.h"


void LedManager::begin(int pin){
  _pin = pin;
  state = false; 
  pinMode(_pin, OUTPUT);
}


void LedManager::on(){
  state = true; 
  digitalWrite(_pin, LOW); 
}

 
void LedManager::off(){
  state = false; 
  digitalWrite(_pin, HIGH);
}


void LedManager::blink(){
  on();
  delay(100);
  off();
  delay(100);
}