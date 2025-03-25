#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include "convertor.h"

enum class TimeData{
  sec, 
  min,
  hr
};

class SystemTime {
public:
  SystemTime(uint8_t address_);
  char memDS3231[32] = { '\0' };
  int timeString[3] = { '\0' };
  String Day = "";
  int Date = 0;
  int Month = 0;
  void GetTime();
  void SetTime(TimeData param, uint8_t data);
  void GetDay();
  void SetDay(uint8_t data);
  void GetDate();
  void SetDate(uint8_t data);
  void GetMonth();
private:
  uint8_t address;
  Convertor conv;
  void WriteToRegister(uint8_t regAddr, uint8_t data);
};

#endif