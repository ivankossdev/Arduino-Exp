#include "ds3231.h"
#include <Wire.h>

SysClock::SysClock(uint8_t address_) {
  this->address = address_;
  Wire.begin();
}

void SysClock::GetTime() {
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
