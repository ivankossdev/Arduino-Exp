#ifndef _SERIALSET_H
#define _SERIALSET_H
#include <Arduino.h>
#include "ds3231.h"
#include "convertor.h"
#include "I2C_Device.h"
#include "ex_eeprom.h"
// #define DS3231 0x68
// #define EEPROM 0x57

class SerialSet {
public:
  SerialSet(unsigned long speed);
  void SetCMD();
  
private:
  SystemTime systime{ DS3231 };
  Convertor conv; 
  void CmdHandler(String &data);
  char cmd[8] = {'\0'};
  int  insertData[3] = {'\0'};
  void setDay(int day);
  void setTime(long time);
  void setDate(long date);
  void formatData(long data, int *intData);
  I2C_Device i2cDev;
};

#endif