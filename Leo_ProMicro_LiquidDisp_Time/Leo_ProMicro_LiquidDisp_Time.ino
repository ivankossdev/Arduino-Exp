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
  for (int i = 1; i < 04; i++) {
    lcdTime.printTime(i + 3, i + 1, i);
    delay(500);
  }
  delay(500);
}
