#include "MyHW595.h"
#include <SPI.h>

// Инициализация шины SPI
MYHW595::MYHW595(uint8_t latchPin){
  this->latchPin = latchPin;
  pinMode(latchPin, OUTPUT);
  SPI.begin();
}

// Отправка данных
void MYHW595::WriteByte(char data){
  digitalWrite(latchPin, LOW);
  SPI.transfer(data); 
  digitalWrite(latchPin, HIGH);
}