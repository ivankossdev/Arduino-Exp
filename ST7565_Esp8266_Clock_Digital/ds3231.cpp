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
  constexpr uint8_t REG_STATUS    = 0x0F;
  constexpr uint8_t REG_TEMP_INT  = 0x11;
  constexpr uint8_t REG_TEMP_FRAC = 0x12;

  // Маски полей
  constexpr uint8_t MASK_DATE  = 0x3F;
  constexpr uint8_t MASK_MONTH = 0x1F;
  constexpr uint8_t MASK_DAY   = 0x07;
  constexpr uint8_t MASK_OSF   = 0x80;  // Oscillator Stop Flag
}

SystemTime::SystemTime(uint8_t address_) {
  this->address = address_;
}

bool SystemTime::WriteToRegister(uint8_t regAddr, uint8_t data) {
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.write(conv.decToBcd((int)data));
  return Wire.endTransmission() == 0;
}

bool SystemTime::ReadRegister(uint8_t regAddr, uint8_t size, uint8_t *data) {
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  if (Wire.endTransmission() != 0) return false;

  uint8_t n = Wire.requestFrom((uint8_t)address, size);
  if (n != size) return false;

  uint8_t c = 0;
  while (Wire.available() && c < size) {
    data[c] = Wire.read();
    c++;
  }
  return c == size;
}

bool SystemTime::initCheck() {
  uint8_t status = 0;
  if (!ReadRegister(REG_STATUS, 1, &status)) {
    timeValidCached = false;
    return false;
  }
  timeValidCached = (status & MASK_OSF) == 0;
  return true;
}

bool SystemTime::refreshTimeValid() {
  uint8_t status = 0;
  if (!ReadRegister(REG_STATUS, 1, &status)) return false;
  timeValidCached = (status & MASK_OSF) == 0;
  return true;
}

bool SystemTime::getTime() {
  uint8_t raw[3] = { 0 };
  if (!ReadRegister(REG_SEC, 3, raw)) return false;
  for (int i = 0; i < 3; i++) {
    timeString[i] = conv.bcdToDec(raw[i]);
  }
  return true;
}

bool SystemTime::SetTime(int hours, int minutes, int seconds) {
  if (!WriteToRegister(REG_SEC,  (uint8_t)seconds)) return false;
  if (!WriteToRegister(REG_MIN,  (uint8_t)minutes)) return false;
  if (!WriteToRegister(REG_HOUR, (uint8_t)hours))   return false;
  clearOscillatorFlag();
  timeValidCached = true;
  return true;
}

bool SystemTime::clearOscillatorFlag() {
  // Read-modify-write регистра 0x0F: сбросить бит 7 (OSF), остальные не трогать.
  uint8_t status = 0;
  if (!ReadRegister(REG_STATUS, 1, &status)) return false;
  status &= ~MASK_OSF;

  Wire.beginTransmission(address);
  Wire.write(REG_STATUS);
  Wire.write(status);
  return Wire.endTransmission() == 0;
}

int SystemTime::getDate() {
  uint8_t raw = 0;
  if (!ReadRegister(REG_DATE, 1, &raw)) return -1;
  return conv.bcdToDec(raw & MASK_DATE);
}

void SystemTime::setDate(uint8_t data) {
  WriteToRegister(REG_DATE, data & MASK_DATE);
}

int SystemTime::getMonth() {
  uint8_t raw = 0;
  if (!ReadRegister(REG_MONTH, 1, &raw)) return -1;
  return conv.bcdToDec(raw & MASK_MONTH);
}

void SystemTime::setMonth(uint8_t data) {
  WriteToRegister(REG_MONTH, data & MASK_MONTH);
}

int SystemTime::getYear() {
  uint8_t raw = 0;
  if (!ReadRegister(REG_YEAR, 1, &raw)) return -1;
  return conv.bcdToDec(raw);
}

void SystemTime::SetYear(uint8_t data) {
  WriteToRegister(REG_YEAR, data);
}

const char* SystemTime::getDay() {
  uint8_t raw = 0;
  if (!ReadRegister(REG_DAY, 1, &raw)) return "Err";
  int day = conv.bcdToDec(raw & MASK_DAY);
  switch (day) {
    case 1: return "Mon";
    case 2: return "Tue";
    case 3: return "Wed";
    case 4: return "Thu";
    case 5: return "Fri";
    case 6: return "Sat";
    case 7: return "Sun";
    default: return "Err";
  }
}

void SystemTime::setDay(uint8_t data) {
  WriteToRegister(REG_DAY, data);
}

const char* SystemTime::getTemperature() {
  uint8_t rawInt = 0;
  if (!ReadRegister(REG_TEMP_INT, 1, &rawInt)) {
    return temp;  // I2C упал — оставляем предыдущее значение
  }
  int8_t intPart = (int8_t)rawInt;

  uint8_t rawFrac = 0;
  ReadRegister(REG_TEMP_FRAC, 1, &rawFrac);  // при ошибке frac = 0
  uint8_t frac = ((rawFrac >> 6) & 0x03) * 25;

  bool negative = (intPart < 0);
  int absInt = negative ? -intPart : intPart;

  int i = 0;
  if (negative) temp[i++] = '-';
  temp[i++] = (char)('0' + (absInt / 10));
  temp[i++] = (char)('0' + (absInt % 10));
  temp[i++] = '.';
  temp[i++] = (char)('0' + (frac / 10));
  temp[i++] = (char)('0' + (frac % 10));
  temp[i]   = '\0';
  return temp;
}