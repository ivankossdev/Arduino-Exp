#include "display.h"

void setup() {
  displayInit();
}

void loop() {
  Test10();
}


void Test10(void) {
  int c0 = 0, c1 = 0, c2 = 0;
  
  for (int i = 0; i <= 999; i++) {
    mylcd.LCDgotoXY(15, 1);
    c2 = i / 100;
    c1 = i / 10 % 10;
    c0 = i % 10;
    mylcd.printf("cnt-%d%d%d", c2, c1, c0);
    delay(500);
    // mylcd.LCDClear(0x00);
  }
}
