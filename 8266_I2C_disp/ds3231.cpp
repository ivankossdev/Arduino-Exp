#include "ds3231.h"
#include <Wire.h>

SystemTime::SystemTime(uint8_t address_) {
  this->address = address_;
  Wire.begin();
}

void SystemTime::GetTime() {
  int c = 0;
  Wire.beginTransmission(address);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(address, (size_t)3, true);

  while (Wire.available()) {
    timeString[c] = conv.FromEightToDec(Wire.read());
    c++;
  }
}

void SystemTime::WriteToRegister(uint8_t regAddr, uint8_t data){
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.write(conv.FromDecToEight((int)data));
  Wire.endTransmission();
}

void SystemTime::SetTime(TimeDate param, uint8_t data){
  switch (param){
    case TimeDate::sec:WriteToRegister(0, data); break;
    case TimeDate::min:WriteToRegister(1, data); break;
    case TimeDate::hr:WriteToRegister(2, data); break;
  }
}

void SystemTime::GetDate(){
  
}