#include "display.h"
#include "dsp_driver.h"

#define ARRAY 8

unsigned char fig[ARRAY] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

DspDriver driver;

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);
}

void loop() {
  ExShiftLift();
  ExShiftRight();
}

void ExShiftLift() {

  // Insert image
  for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 6; y++) {
      driver.insertFig(fig, ARRAY, x * ARRAY, y * ARRAY);
    }
  }

  // Show image
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);

  delay(2000);

  // Left shift example
  for (int i = 0; i < 40; i++) {
    for (int i_ = 0; i_ < 6; i_++) {
      driver.shiftLeft(i_, 80);
    }
    mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
    delay(100);
  }

  delay(1000);

  driver.clearEx();
}

void ExShiftRight() {
  // Insert image
  for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 6; y++) {
      driver.insertFig(fig, ARRAY, x * ARRAY, y * ARRAY);
    }
  }

  // Show image
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);

  delay(2000);

  // Left shift example
  // for (int i = 0; i < 80; i++) {
  //   for (int i_ = 0; i_ < 6; i_++) {
  //     driver.shiftLeft(i_, 80);
  //   }
  //   mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
  //   delay(100);
  // }

  // delay(1000);

  driver.clearEx();
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
  delay(1000);
}
