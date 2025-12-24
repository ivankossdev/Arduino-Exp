#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include "convertor.h"

enum class TimeData {
  sec,
  min,
  hr
};

class SystemTime {
public:
  SystemTime(uint8_t address_);


private:
  uint8_t address;
  Convertor conv;
};

#endif