#ifndef _SERIALSET_H
#define _SERIALSET_H
#include <Arduino.h>
#include "ds3231.h"
#include "convertor.h"
#include "AT24xx.h"

class SerialSet {
public:
  SerialSet(unsigned long speed_, SystemTime& systime_, At24& at24_);
  void Init();
  void SetCMD();

private:
  SystemTime& systime;
  At24&       at24;
  Convertor   conv;
  unsigned long speed = 0;

  void CmdHandler(String &data);
  void setDay(int day);
  void setTime(long time);
  void setDate(long date);
  void formatData(long data, int *intData);
  void showDate();
  void showTime();
};

#endif