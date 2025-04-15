#include "display.h"
#include "dsp_driver.h"

unsigned char fig[8] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };
DspDriver driver;

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {
  driver.insertFig(fig, 8, 40, 20);
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
  delay(5000);
  driver.clearEx();
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
  delay(500);
}

void test(unsigned char *img) {
  for (int y = 2; y < 7; y++) {
    for (int x = 4; x < 12; x++) {
      driver.insertFig(img, 8, x * 5, y * 5);
    }
  }
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
  delay(5000);

  driver.clearEx();
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
  delay(500);
}