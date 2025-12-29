#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "MyMillis.h"

char data[32] = { '\0' };
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
MyMillis interval_1(250);
MyMillis interval_2(1000);
int count_1 = 0;
int count_2 = 0;

void printCount(int posY, int &c) {
  lcd.setCursor(0, posY);
  if (c > 99) {
    c = 0;
    sprintf(data, "Count [%d] ", c);
  } else {
    sprintf(data, "Count [%d]", c);
  }
  lcd.print(data);
  c++;
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  interval_1.tic(printCount, 0, count_1);
  interval_2.tic(printCount, 1, count_2);
}
