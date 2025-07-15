#ifndef __MYHW595_H
#define __MYHW595_H

#include "Arduino.h"

/*
Arduino  Leonardo
         board ->    <- HC595
         MOSI 16    14 | DS    | Data pin
         SCK  15    11 | SH_CP | Clock
         SS   10    12 | ST_CP | Storage Registet input
ESP8266
         board ->    <- HC595
         MOSI D7    14 | DS    | Data pin
         SCK  D5    11 | SH_CP | Clock
         SS   D4    12 | ST_CP | Storage Registet input
*/

class MYHW595 {
  
public:
  MYHW595(uint8_t latchPin = 10);
  void WriteByte(char data);

private:
  uint8_t latchPin; 
};

#endif