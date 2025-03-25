#ifndef _BUSSET_H
#define _BUSSET_H
#include <Arduino.h>
#include "ds3231.h"
#define DS3231 0x68

/*
  CMD:
    Set time: sett235959 (23:59:59)
    Set day:  setd2      (day 1 - 7)
    Set date: setdt25    (1-31)
*/

enum class SetCmd{
  time,
  day,
  date,
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