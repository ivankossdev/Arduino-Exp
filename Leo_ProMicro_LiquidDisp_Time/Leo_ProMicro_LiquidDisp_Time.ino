#include <Wire.h>
#include "lcdTime.h"

#define printByte(args) write(args);

/* 
  Leonardo pro micro
  2 SDA
  3 SCL
 */

int lcdColumns = 16;
int lcdRows = 2;

LcdTime lcdTime(0x27, lcdColumns, lcdRows);

void setup() {
  lcdTime.init();
  lcdTime.backlight();
  lcdTime.clear();
}

void loop() {
  for (int i = 0; i < 24; i++) {
    lcdTime.printTime(i, i, i);
    delay(500);
  }
  delay(2500);
  lcdTime.clear();
  delay(500);
}
