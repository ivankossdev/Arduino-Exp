#ifndef _BUSSET_H
#define _BUSSET_H
#include <Arduino.h>
#include "ds3231.h"
#define DS3231 0x68

enum class SetCmd{
  time,
  day,
  mounth,
  year,
  nodata
};

class BusSet {
public:
  BusSet();
  void SetSerial();
private:
  SystemTime systime{ DS3231 };
  SetCmd CheckCMD(String cmd);
  uint8_t TwoCharToInt(char c1, char c0);
};

#endif