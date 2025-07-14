#include "MyHW595.h"

/*
Arduino  Leonardo
         board ->    <- HC595
         MOSI 16    14 | DS    | Data pin
         SCK  15    11 | SH_CP | Clock
         SS   10    12 | ST_CP | Storage Registet input
*/

MYHW595 hw595;

void ShiftLeft();
void ShiftRight();

void setup() {}

void loop() {
  ShiftLeft();
  delay(100);
  ShiftRight();
  delay(100);
}

void ShiftLeft() {
  for (uint8_t i = 0; i < 8; i++) {
    hw595.WriteByte(1 << i);
    delay(50);
  }
}

void ShiftRight(){
  for(int i = 7; i >= 0; i--){
    hw595.WriteByte(1 << i);
    delay(50);
  }
}