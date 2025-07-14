#ifndef __MYHW595_H
#define __MYHW595_H

#include "Arduino.h"

class MYHW595 {
  
public:
  MYHW595(uint8_t latchPin = 10);
  void WriteByte(char data);

private:
  uint8_t latchPin; 
};

#endif