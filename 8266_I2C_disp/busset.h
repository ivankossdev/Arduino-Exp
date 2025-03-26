#ifndef _BUSSET_H
#define _BUSSET_H
#include <Arduino.h>
#include "ds3231.h"
#define DS3231 0x68

/*
  CMD:
    Set time:  settm235959 (23:59:59)
    Set day:   setdy2      (day 1 - 7)
    Set date:  setdt25     (01-31)
    Set month: setmn04     (01 - 12)
    Set year:  setyr25     (00-99) 
*/

enum class SetCmd{
  time,
  day,
  date,
  month,
  year,
  nodata
};

class BusSet {
public:
  BusSet();
  bool isSet = false; 
  void SetSerial();
private:
  SystemTime systime{ DS3231 };
  SetCmd CheckCMD(String cmd);
  uint8_t TwoCharToInt(char c1, char c0);
};

#endif