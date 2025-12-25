#ifndef _SERIALSET_H
#define _SERIALSET_H
#include <Arduino.h>
#include "ds3231.h"
#include "convertor.h"
#define DS3231 0x68


/*
  CMD:
    Set time:  settm235959 (23:59:59)
    Set day:   setdy2      (day 1 - 7)
    Set date:  setdt25     (01-31)
    Set month: setmn04     (01 - 12)
    Set year:  setyr25     (00-99) 
*/

class SerialSet {
public:
  SerialSet(unsigned long speed);
  void SetCMD();
private:
  SystemTime systime{ DS3231 };
  void CmdHandler(String &data);
  uint8_t TwoCharToInt(char c1, char c0);
  char cmd[8] = {'\0'};
  void setDay(int day);
  Convertor conv; 
};

#endif