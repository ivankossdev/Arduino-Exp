#ifndef __CONNECTDATA_H
#define __CONNECTDATA_H

#include "Arduino.h"
#include <EEPROM.h>

class ConnectData {
public:
  void enterSerialDataInArray(char *data, String message);
  char ssid[32] = {'\0'};
  char pass[16] = {'\0'};

private:
  String serialData;
};

static ConnectData connectData;
#endif