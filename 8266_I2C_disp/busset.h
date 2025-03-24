#ifndef _BUSSET_H
#define _BUSSET_H
#include <Arduino.h>
#include "ds3231.h"
#define DS3231 0x68

class Busset {
public:
  Busset();
  void SetSerial();
private:
  SystemTime systime{ DS3231 };
  bool CheckCMD(String cmd);
  uint8_t TwoCharToInt(char c1, char c0);
};

#endif