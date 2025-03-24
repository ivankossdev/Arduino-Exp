#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include "convertor.h"

enum class TimeDate{
  sec, 
  min,
  hr
};

class SystemTime {
public:
  SystemTime(uint8_t address_);
  char memDS3231[32] = { '\0' };
  int timeString[3] = { '\0' };
  int Day = 0; 
  void GetTime();
  void SetTime(TimeDate param, uint8_t data);
  void GetDay();
  void SetDay(uint8_t data);
private:
  uint8_t address;
  Convertor conv;
  void WriteToRegister(uint8_t regAddr, uint8_t data);
};

#endif