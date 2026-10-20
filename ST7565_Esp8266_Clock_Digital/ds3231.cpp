#include "ds3231.h"
#include <Wire.h>

SystemTime::SystemTime(uint8_t address_) {
  this->address = address_;
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

void SystemTime::SetTime(int *time){
  for(int i = 0; i < 3; i++)
    WriteToRegister(i, time[i]);
}

void SystemTime::getDate() {
  ReadRegister(4, 1, OneRegisterData);
  Date = conv.FromEightToDec(OneRegisterData[0] & 0x3f);
}

void SystemTime::setDate(uint8_t data) {
  WriteToRegister(4, data & 0x3f);
}

void SystemTime::getMonth() {
  ReadRegister(5, 1, OneRegisterData);
  Month = conv.FromEightToDec(OneRegisterData[0] & 0x1f);
}

void SystemTime::setMonth(uint8_t data) {
  WriteToRegister(5, data & 0x1f);
}

void SystemTime::getYear() {
  ReadRegister(6, 1, OneRegisterData);
  Year = conv.FromEightToDec(OneRegisterData[0]);
}

void SystemTime::SetYear(uint8_t data) {
  WriteToRegister(6, data);
}

void SystemTime::getDay() {
  int day = 0;
  ReadRegister(3, 1, OneRegisterData);
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
    default: Day = "Err"; break;
  }
}

void SystemTime::setDay(uint8_t data) {
  WriteToRegister(3, data);
}

void SystemTime::getTemperature() {
  // Register 0x11: signed 8-bit integer part (NOT BCD)
  ReadRegister(0x11, 1, OneRegisterData);
  int8_t intPart = (int8_t)OneRegisterData[0];

  // Register 0x12 bits 7:6: fractional part in 0.25 °C steps (0, 25, 50, 75)
  ReadRegister(0x12, 1, OneRegisterData);
  uint8_t frac = (((uint8_t)OneRegisterData[0]) >> 6) * 25;

  bool negative = (intPart < 0);
  int absInt = negative ? -intPart : intPart;

  int i = 0;
  if (negative)       Temp[i++] = '-';
  Temp[i++] = (char)('0' + (absInt / 10));
  Temp[i++] = (char)('0' + (absInt % 10));
  Temp[i++] = '.';
  Temp[i++] = (char)('0' + (frac / 10));
  Temp[i++] = (char)('0' + (frac % 10));
  Temp[i]   = '\0';
}