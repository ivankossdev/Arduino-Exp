#ifndef _EX_EEPROM_
#define _EX_EEPROM_
#include <Arduino.h>
#include <Wire.h>

#define EEPROM 0x57

class ExEeprom {
public:
  ExEeprom(uint8_t address_);
  
private:
  uint8_t address;
};

#endif