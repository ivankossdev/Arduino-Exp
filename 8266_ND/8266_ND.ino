#include "display.h"

unsigned char fig[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {
  for(int i = 0; i < 8; i++){
    insertFig(fig, 8, i * 10);
    delay(500);
    clearEx();
    delay(500);
  }
}
