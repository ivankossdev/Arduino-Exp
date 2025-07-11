#ifndef __MY595_H
#define __MY595_H

#include "Arduino.h"

class MY595 {
  
public:
  MY595(uint8_t clk = 5, uint8_t st_cp = 6, uint8_t data = 7);
  void WriteByte(char _byte);

private:
  uint8_t clk; 
  uint8_t st_cp; 
  uint8_t data;
  void WriteBit(char _bit);
};

#endif