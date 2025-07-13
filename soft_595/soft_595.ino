/*
  Arduino board >>    << 595 board default
            CLK  5    14 | DS    | Data pin
            STcp 6    11 | SH_CP | Clock
            Data 7    12 | ST_CP | Storage Registet input
*/
#include "My595.h"

MY595 my595(14, 16, 10);
void setup() {}

void loop() {
  for(int i = 0; i < 8; i++){
    my595.WriteByte(1 << i);
    delay(100);
  }
}
