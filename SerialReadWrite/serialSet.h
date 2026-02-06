#ifndef _SERIALSET_H
#define _SERIALSET_H
#include <Arduino.h>

class SerialSet {
public:
  SerialSet(unsigned long speed_);
  void Init();
  void SetCMD();
  
private:
  unsigned long speed = 0; 
  void CmdHandler(String &data);
  char cmd[8] = {'\0'};
  int  insertData[3] = {'\0'};
};

#endif