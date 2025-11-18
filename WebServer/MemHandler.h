#ifndef __MEMHANDLER_H
#define __MEMHANDLER_H

#include "Arduino.h"
#include <EEPROM.h>
#define MEMSIZE 512

class Memory{
  public:
    Memory();
    bool writeString(String str, int memoryPosition);
    void readString(int memoryPosition);
    void clearAllMemory();
    void clearBuffer();
    char buffer[MEMSIZE] = {'\0'};
  private:
   void commit(String message);
};
 
extern Memory memory;

#endif