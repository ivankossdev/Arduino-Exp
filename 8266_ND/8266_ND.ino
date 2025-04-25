#include "display.h"
#include "dsp_driver.h"

unsigned char fig[8] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

DspDriver driver;

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {
  Example0();
}

void Example0() {
  for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 6; y++) {
      driver.insertFig(fig, 8, x * 8, y * 8);
    }
  }

  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);

  for (int y = 2; y < 7; y++) {
    for (int x = 4; x < 12; x++) {
      driver.insertFig(fig, 8, x * 5, y * 5);
    }
    mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
    delay(50);
  }

  delay(2000);

  for (int i = 0; i < 80; i++) {
    for (int i_ = 0; i_ < 6; i_++) {
      driver.shiftLeft(i_, 80);
    }
    mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
    delay(50);
  }

  delay(1000);

  driver.clearEx();
}