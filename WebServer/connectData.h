#ifndef __CONNECTDATA_H
#define __CONNECTDATA_H

#include "Arduino.h"
#include <EEPROM.h>
#include "MemHandler.h"

#define ENTERSSID 0
#define ENTERPASS 32
/* Настройки сети */
#define IPCNT 4       // Шаг для чтения из памяти IP адресов
#define IPACTION 48   // адрес сервера событий 
#define IPADDRESS 52  // адрес контроллера 
#define MASK 56       // маска 
#define GETWAY 60     // шлюз 

#define ENDPOSITION 64

class ConnectData {
public:
  void enterSSID();
  void enterSSID(char *memData_);
  void enterPASS();
  void enterPASS(char *memData_);
  void writeMemoryNetData(int *data, int arrayPos);
  void writeMemoryToArrayData(int arrayPos);
  char ssid[32] = {'\0'};
  char pass[32] = {'\0'};
  int ip[4] = { 0 };
  int gateway[4] = { 0 };
  int subnet[4] = { 0 };
  int actionServer[4] = { 0 };
  String urlClientAction;
  int rawData[ENDPOSITION] = { 0 };


private:
  void enterSerialDataInArray(char *data, String message);
  void writeDataInArray(char *data, char *memData);
  String serialData;
};

extern ConnectData connectData;
#endif