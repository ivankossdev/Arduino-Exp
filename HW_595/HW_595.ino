#include "MyHW595.h"

/*
Leonardo
Arduino board ->    <- HC595
         MOSI 16    14 | DS    | Data pin
         SCK  15    11 | SH_CP | Clock
         SS   10    12 | ST_CP | Storage Registet input
*/

MYHW595 hw595;

void setup() { }

void loop() {
  for(uint8_t i = 0; i < 8; i++){
    hw595.WriteByte(1 << i);
    delay(100);
  }
}
