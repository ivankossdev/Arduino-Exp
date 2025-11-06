#ifndef __CONNECTDATA_H
#define __CONNECTDATA_H

#include "Arduino.h"
#include <EEPROM.h>

class ConnectData {
public:
  void enterSSID();
  void enterSSID(char *memData_);
  void enterPASS();
  void enterPASS(char *memData_);
  char ssid[32] = {'\0'};
  char pass[32] = {'\0'};


private:
  void enterSerialDataInArray(char *data, String message);
  void writeDataInArray(char *data, char *memData);
  String serialData;
};

extern ConnectData connectData;
#endif