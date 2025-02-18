#ifndef __MEMHANDLER_H
#define __MEMHANDLER_H

#include "Arduino.h"
#include <EEPROM.h>

class Memory{

  public:
    Memory();
    void writeString();
    void readString();
    void clearString();
    char example[512] = {'\0'};
};

#endif