#include "ds3231.h"
#include <Wire.h>

SystemTime::SystemTime(uint8_t address_) {
  this->address = address_;
  Wire.begin();
}

void SystemTime::WriteToRegister(uint8_t regAddr, uint8_t data) {
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.write(conv.FromDecToEight((int)data));
  Wire.endTransmission();
}

void SystemTime::ReadRegister(uint8_t regAddr, uint8_t size, int *data) {

  int c = 0;
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.endTransmission();
  Wire.requestFrom(address, size);

  while (Wire.available()) {
    data[c] = Wire.read();
    c++;
  }
}

void SystemTime::getTime() {
  ReadRegister(0, 3, timeString);
  for (int i = 0; i < 3; i++) {
    timeString[i] = conv.FromEightToDec(timeString[i]);
  }
}

void SystemTime::getDate() {
  ReadRegister(4, 1, OneRegisterData);
  Date = conv.FromEightToDec(OneRegisterData[0] & 0x3f);
}

void SystemTime::getMonth() {
  ReadRegister(5, 1, OneRegisterData);
  Month = conv.FromEightToDec(OneRegisterData[0] & 0x1f);
}

void SystemTime::getYear() {
  ReadRegister(6, 1, OneRegisterData);
  Year = conv.FromEightToDec(OneRegisterData[0]);
}

void SystemTime::getDay() {
  int day = 0;
  ReadRegister(3, (size_t)1, OneRegisterData);
  day = conv.FromEightToDec(OneRegisterData[0] & 0x07);
  Day = ""; 
  switch (day) {
    case 1: Day = "Mon"; break;
    case 2: Day = "Tue"; break;
    case 3: Day = "Wed"; break;
    case 4: Day = "Thu"; break;
    case 5: Day = "Fri"; break;
    case 6: Day = "Sat"; break;
    case 7: Day = "Sun"; break;
    default: Day = "Error param"; break;
  }
}
