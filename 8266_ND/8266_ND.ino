#include "display.h"

unsigned char fig[8] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {

  for (int y = 0; y < 18; y++) {
    insertFig(fig, 8, 2, y);
    delay(100);
  }
  // insertFig(fig, 8, 0, 0);
  // delay(100);

  // insertFig(fig, 8, 8, 8);
  // delay(100);

  // insertFig(fig, 8, 0, 9);
  // delay(100);
  
  // insertFig(fig, 8, 8, 17);
  delay(5000);

  clearEx();
}
