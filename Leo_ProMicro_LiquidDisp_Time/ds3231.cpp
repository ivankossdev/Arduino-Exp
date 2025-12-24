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
  ReadRegister(4, (size_t)1, OneRegisterData);
  Date = conv.FromEightToDec(OneRegisterData[0] & 0x3f);
}
