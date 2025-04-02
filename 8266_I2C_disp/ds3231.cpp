#include "ds3231.h"
#include <Wire.h>

void SystemTime::WriteToRegister(uint8_t regAddr, uint8_t data) {
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.write(conv.FromDecToEight((int)data));
  Wire.endTransmission();
}

void SystemTime::ReadRegister(uint8_t regAddr, size_t size, int *data) {

  int c = 0;
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.endTransmission();
  Wire.requestFrom(address, (size_t)size, true);

  while (Wire.available()) {
    data[c] = Wire.read();
    c++;
  }
}

void SystemTime::ClearMemStr(char *data, int cnt) {
  for (int cnt = 0; cnt < 32; cnt++) {
    data[cnt] = '\0';
  }
}

SystemTime::SystemTime(uint8_t address_) {
  this->address = address_;
  Wire.begin();
}

void SystemTime::GetTime() {
  ReadRegister(0, (size_t)3, timeString);
  for (int i = 0; i < 3; i++) {
    timeString[i] = conv.FromEightToDec(timeString[i]);
  }
}

void SystemTime::SetTime(TimeData param, uint8_t data) {
  switch (param) {
    case TimeData::sec: WriteToRegister(0, data); break;
    case TimeData::min: WriteToRegister(1, data); break;
    case TimeData::hr: WriteToRegister(2, data); break;
  }
}

void SystemTime::GetDay() {
  int day = 0;
  ReadRegister(3, (size_t)1, OneRegisterData);
  day = conv.FromEightToDec(OneRegisterData[0] & 0x07);
  Day.clear();
  switch (day) {
    case 1: Day = "Monday"; break;
    case 2: Day = "Tuesday"; break;
    case 3: Day = "Wednesday"; break;
    case 4: Day = "Thursday "; break;
    case 5: Day = "Friday"; break;
    case 6: Day = "Saturday"; break;
    case 7: Day = "Sunday"; break;
    default: Day = "Error param"; break;
  }
}

void SystemTime::SetDay(uint8_t data) {
  WriteToRegister(3, data);
}

void SystemTime::GetDate() {
  ReadRegister(4, (size_t)1, OneRegisterData);
  Date = conv.FromEightToDec(OneRegisterData[0] & 0x3f);
}

void SystemTime::SetDate(uint8_t data) {
  WriteToRegister(4, data & 0x3f);
}

void SystemTime::GetMonth() {
  ReadRegister(5, (size_t)1, OneRegisterData);
  Month = conv.FromEightToDec(OneRegisterData[0] & 0x1f);
}

void SystemTime::SetMonth(uint8_t data) {
  WriteToRegister(5, data & 0x1f);
}

void SystemTime::GetYear() {
  ReadRegister(6, (size_t)1, OneRegisterData);
  Year = conv.FromEightToDec(OneRegisterData[0]);
}

void SystemTime::SetYear(uint8_t data) {
  WriteToRegister(6, data);
}

void SystemTime::GetTemperature() {
  ReadRegister(0x11, (size_t)1, OneRegisterData);
  int t0x11 = conv.FromEightToDec(OneRegisterData[0]);
  Temp[0] = conv.TenFormat(t0x11) | 0x30;
  Temp[1] = conv.OneFormat(t0x11) | 0x30;

  ReadRegister(0x12, (size_t)1, OneRegisterData);
  int t0x12 = (OneRegisterData[0] >> 6) * 25;
  Temp[3] = conv.TenFormat(t0x12) | 0x30;
  Temp[4] = conv.OneFormat(t0x12) | 0x30;
}
