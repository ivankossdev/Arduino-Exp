#ifndef __MY595_H
#define __MY595_H

#include "Arduino.h"

class MY595 {
  
public:
  MY595(uint8_t clk = 4, uint8_t st_cp = 5, uint8_t data = 6);
private:
  uint8_t clk; 
  uint8_t st_cp; 
  uint8_t data;
};

#endif