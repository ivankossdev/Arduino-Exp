#ifndef __MEMHANDLER_H
#define __MEMHANDLER_H

#include "Arduino.h"
#include <EEPROM.h>

class Memory{

  public:
    Memory();
    bool writeString();
    void readString();
    void clearString();
    char buffer[512] = {'\0'};
};

#endif