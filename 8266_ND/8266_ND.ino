#include "display.h"

void setup() {
  displayInit();
}

void loop() {
  mylcd.LCDgotoXY(0, 0);
  drawFullPixel(0xff);
  drawFullPixel(0x00);
}
