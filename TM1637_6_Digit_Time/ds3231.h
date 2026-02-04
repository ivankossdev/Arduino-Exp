#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include "convertor.h"

#define DS3231 0x68

enum class TimeData {
  sec,
  min,
  hr
};

class SystemTime {
public:
  SystemTime(uint8_t address_);
  void getTime();
  void SetTime(int *time);
  int timeString[3] = { '\0' };
  int Date = 0;
  int Month = 0;
  int Year = 0;
  String Day = "";
  char Temp[6] = { '0', '0', '.',  '0', '0'};
  void getDate();
  void setDate(uint8_t data);
  void getMonth();
  void setMonth(uint8_t data);
  void getYear();
  void SetYear(uint8_t data);
  void getDay();
  void setDay(uint8_t data);
  void getTemperature();

private:
  uint8_t address;
  Convertor conv;
  int OneRegisterData[1] = { '\0' };
  void WriteToRegister(uint8_t regAddr, uint8_t data);
  void ReadRegister(uint8_t regAddr, uint8_t size, int *data);
};

#endif