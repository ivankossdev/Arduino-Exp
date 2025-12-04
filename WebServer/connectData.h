#ifndef __CONNECTDATA_H
#define __CONNECTDATA_H

#include "Arduino.h"
#include <EEPROM.h>

#define ENTERSSID 0
#define ENTERPASS 32
#define IPACTION 48
#define IPCNT 4
#define IPADDRESS 52 
#define MASK 56
#define ENDPOSITION 64

class ConnectData {
public:
  void enterSSID();
  void enterSSID(char *memData_);
  void enterPASS();
  void enterPASS(char *memData_);
  char ssid[32] = {'\0'};
  char pass[32] = {'\0'};
  int ip[4] = { 0 };
  int gateway[4] = { 0 };
  int subnet[4] = { 0 };
  int actionServer[4] = { 0 };
  String urlClientAction;


private:
  void enterSerialDataInArray(char *data, String message);
  void writeDataInArray(char *data, char *memData);
  String serialData;
};

extern ConnectData connectData;
#endif