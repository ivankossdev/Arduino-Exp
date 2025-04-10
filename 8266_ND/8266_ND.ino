#include "display.h"

unsigned char fig[8] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {

  for (int y = 0; y < 3; y++) {
    for (int x = 5; x < 12; x++) {
      insertFig(fig, 8, x * 5, y * 5);
      delay(50);
    }
  }

  delay(5000);

  clearEx();
  delay(100);
}
