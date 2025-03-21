#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include "convertor.h"

class SysClock {
public:
  SysClock(uint8_t address_);
  char memDS3231[32] = { '\0' };
  int timeString[3] = { '\0' };
  void GetTime();

protected:
  uint8_t address;
private:
  Convertor conv;
};

#endif