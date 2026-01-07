#include "ex_eeprom.h"

ExEeprom::ExEeprom(uint8_t address_){
  this->address = address_;
  Wire.begin();
}





