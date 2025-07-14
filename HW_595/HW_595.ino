#include "MyHW595.h"

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
// Connection
#define SS D4

#define delay_1 100
#define delay_2 50

MYHW595 hw595(SS);

void ShiftLeft();
void ShiftRight();

void setup() {}

void loop() {
  ShiftLeft();
  delay(delay_1);
  ShiftRight();
  delay(delay_1);
}

void ShiftLeft() {
  for (uint8_t i = 0; i < 8; i++) {
    hw595.WriteByte(1 << i);
    delay(delay_2);
  }
}

void ShiftRight(){
  for(int i = 7; i >= 0; i--){
    hw595.WriteByte(1 << i);
    delay(delay_2);
  }
}