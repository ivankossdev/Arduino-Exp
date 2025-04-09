#include "display.h"

unsigned char fig[8] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {
  insertFig(fig, 8, 10, 5);
    delay(1000);
    clearEx();
    delay(500);
  // for(int i = 0; i < 8; i++){
  //   insertFig(fig, 8, i * 10, 1);
  //   delay(500);
  //   clearEx();
  // }
}
