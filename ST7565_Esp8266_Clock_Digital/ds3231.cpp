#include "ds3231.h"
#include <Wire.h>

namespace {
  // Регистры DS3231
  constexpr uint8_t REG_SEC       = 0x00;
  constexpr uint8_t REG_MIN       = 0x01;
  constexpr uint8_t REG_HOUR      = 0x02;
  constexpr uint8_t REG_DAY       = 0x03;
  constexpr uint8_t REG_DATE      = 0x04;
  constexpr uint8_t REG_MONTH     = 0x05;
  constexpr uint8_t REG_YEAR      = 0x06;
  constexpr uint8_t REG_TEMP_INT  = 0x11;
  constexpr uint8_t REG_TEMP_FRAC = 0x12;

  // Маски полей
  constexpr uint8_t MASK_DATE  = 0x3F;
  constexpr uint8_t MASK_MONTH = 0x1F;
  constexpr uint8_t MASK_DAY   = 0x07;
}

SystemTime::SystemTime(uint8_t address_) {
  this->address = address_;
}

void SystemTime::WriteToRegister(uint8_t regAddr, uint8_t data) {
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.write(conv.decToBcd((int)data));
  Wire.endTransmission();
}

void SystemTime::ReadRegister(uint8_t regAddr, uint8_t size, uint8_t *data) {
  uint8_t c = 0;
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)address, size);

  while (Wire.available() && c < size) {
    data[c] = Wire.read();
    c++;
  }
}

void SystemTime::getTime() {
  uint8_t raw[3] = { 0 };
  ReadRegister(REG_SEC, 3, raw);
  for (int i = 0; i < 3; i++) {
    timeString[i] = conv.bcdToDec(raw[i]);
  }
}

void SystemTime::SetTime(int hours, int minutes, int seconds) {
  WriteToRegister(REG_SEC,  (uint8_t)seconds);
  WriteToRegister(REG_MIN,  (uint8_t)minutes);
  WriteToRegister(REG_HOUR, (uint8_t)hours);
}

void SystemTime::getDate() {
  uint8_t raw = 0;
  ReadRegister(REG_DATE, 1, &raw);
  Date = conv.bcdToDec(raw & MASK_DATE);
}

void SystemTime::setDate(uint8_t data) {
  WriteToRegister(REG_DATE, data & MASK_DATE);
}

void SystemTime::getMonth() {
  uint8_t raw = 0;
  ReadRegister(REG_MONTH, 1, &raw);
  Month = conv.bcdToDec(raw & MASK_MONTH);
}

void SystemTime::setMonth(uint8_t data) {
  WriteToRegister(REG_MONTH, data & MASK_MONTH);
}

void SystemTime::getYear() {
  uint8_t raw = 0;
  ReadRegister(REG_YEAR, 1, &raw);
  Year = conv.bcdToDec(raw);
}

void SystemTime::SetYear(uint8_t data) {
  WriteToRegister(REG_YEAR, data);
}

void SystemTime::getDay() {
  uint8_t raw = 0;
  ReadRegister(REG_DAY, 1, &raw);
  int day = conv.bcdToDec(raw & MASK_DAY);
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
  WriteToRegister(REG_DAY, data);
}

void SystemTime::getTemperature() {
  // Регистр 0x11 — знаковое 8-битное целое (НЕ BCD)
  uint8_t rawInt = 0;
  ReadRegister(REG_TEMP_INT, 1, &rawInt);
  int8_t intPart = (int8_t)rawInt;

  // Регистр 0x12, биты 7:6 — дробная часть с шагом 0.25 °C (0, 25, 50, 75)
  uint8_t rawFrac = 0;
  ReadRegister(REG_TEMP_FRAC, 1, &rawFrac);
  uint8_t frac = ((rawFrac >> 6) & 0x03) * 25;

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