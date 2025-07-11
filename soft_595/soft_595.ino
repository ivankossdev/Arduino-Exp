/*
  Arduino board >>    << 595 board
            CLK  5    --> 11 Clock 
            STcp 6    --> 12 Storage Registet input 
            Data 7    --> 14 Data pin
*/
#include "My595.h"

MY595 my595;
void setup() {}

void loop() {
  for(int i = 0; i < 8; i++){
    my595.WriteByte(1 << i);
    delay(100);
  }
}
