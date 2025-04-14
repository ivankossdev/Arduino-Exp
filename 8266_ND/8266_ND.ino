#include "display.h"
#include "dsp_driver.h"

unsigned char fig[8] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {
  test(fig);
}

void test(unsigned char *img) {
  for (int y = 2; y < 7; y++) {
    for (int x = 4; x < 12; x++) {
      insertFig(img, 8, x * 5, y * 5);
    }
  }
  mylcd.LCDCustomChar(ex, sizeof(ex) / sizeof(unsigned char), LCDPadding_None, true);
  delay(5000);

  clearEx();
  mylcd.LCDCustomChar(ex, sizeof(ex) / sizeof(unsigned char), LCDPadding_None, true);
  delay(500);
}