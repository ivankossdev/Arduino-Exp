#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "I2C_Device.h"
#define printByte(args) write(args);

/* 
  Leonardo pro micro
  2 SDA
  3 SCL
 */

int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
I2C_Device search;

byte customChar[] = {
  B11011,
  B10001,
  B10101,
  B10101,
  B10101,
  B10101,
  B10001,
  B11011
};

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Display test");
  lcd.setCursor(0, 1);
  lcd.print(search.searchDevice());
  delay(1000);
  lcd.clear();
  lcd.createChar(0, customChar);
  lcd.home();
}


void loop() {
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 0);
    lcd.printByte(0);
    lcd.setCursor(i, 1);
    lcd.printByte(0);
    delay(100);
  }


  delay(1000);
  lcd.clear();
  delay(1000);
}
