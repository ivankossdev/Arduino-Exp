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

void SystemTime::WriteToRegister(uint8_t regAddr, uint8_t data) {
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.write(conv.FromDecToEight((int)data));
  Wire.endTransmission();
}

void SystemTime::SetTime(TimeData param, uint8_t data) {
  switch (param) {
    case TimeData::sec: WriteToRegister(0, data); break;
    case TimeData::min: WriteToRegister(1, data); break;
    case TimeData::hr: WriteToRegister(2, data); break;
  }
}

void SystemTime::GetDay() {
 
  Wire.beginTransmission(address);
  Wire.write(3);
  Wire.endTransmission();
  Wire.requestFrom(address, (size_t)1, true);

  while (Wire.available()) {
    Day = conv.FromEightToDec(Wire.read()) & 0x07;
  }
}

void SystemTime::SetDay(uint8_t data){
  WriteToRegister(3, data);
}

void SystemTime::GetDate(){
  Wire.beginTransmission(address);
  Wire.write(4);
  Wire.endTransmission();
  Wire.requestFrom(address, (size_t)1, true);

  while (Wire.available()) {
    Date = conv.FromEightToDec(Wire.read() & 0x3f);
  }
}

void SystemTime::SetDate(uint8_t data){
  WriteToRegister(4, data & 0x3f);
}








