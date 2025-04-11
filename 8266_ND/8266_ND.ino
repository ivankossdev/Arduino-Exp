#include "display.h"

unsigned char fig[8] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {
  insertFig(fig, 8, 0, 0);  // insert x=0 y=0
  insertFig(fig, 8, 9, 0);  // insert x=9 y=0
  insertFig(fig, 8, 18, 0);  // insert x=18 y=0

  insertFig(fig, 8, 0, 9);  // insert x=0 y=9
  insertFig(fig, 8, 9, 9);  // insert x=9 y=9
  insertFig(fig, 8, 18, 9);  // insert x=18 y=9

  insertFig(fig, 8, 0, 18);  // insert x=0 y=18
  insertFig(fig, 8, 9, 18);  // insert x=9 y=18
  insertFig(fig, 8, 18, 18);  // insert x=18 y=18

  delay(5000);

  clearEx();
  delay(100);
}

void test(unsigned char *img) {
  for (int y = 0; y < 3; y++) {
    for (int x = 4; x < 12; x++) {
      insertFig(img, 8, x * 5, y * 5);
      delay(50);
    }
  }

  delay(5000);

  clearEx();
  delay(500);
}