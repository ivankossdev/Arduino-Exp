#include "display.h"
#include "dsp_driver.h"

#define ARRAY 8

unsigned char fig[ARRAY] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };

DspDriver driver;

void setup() {
  displayInit();
  mylcd.LCDgotoXY(0, 0);

  // Insert image
  for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 6; y++) {
      driver.insertFig(fig, ARRAY, x * ARRAY, y * ARRAY);
    }
  }
}

void loop() {
  ExShiftRight();
  ExShiftLeft();
}

void ExShiftLeft() {

  // Show image
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);

  delay(1000);

  // Left shift example
  for (int i = 0; i < 4; i++) {
    for (int i_ = 0; i_ < 6; i_++) {
      driver.shiftLeft(i_);
    }
    mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
    delay(100);
  }

  delay(1000);
}

void ExShiftRight() {

  // Show image
  mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);

  delay(1000);

  // Right shift example
  for (int i = 0; i < 4; i++) {
    for (int i_ = 0; i_ < 6; i_++) {  // i_ < 6
      driver.shifRigth(i_);
    }
    mylcd.LCDCustomChar(driver.ex, sizeof(driver.ex) / sizeof(unsigned char), LCDPadding_None, true);
    delay(100);
  }

  delay(1000);
}
