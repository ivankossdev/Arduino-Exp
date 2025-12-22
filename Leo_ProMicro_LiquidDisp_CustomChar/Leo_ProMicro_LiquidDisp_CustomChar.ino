#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define printByte(args) write(args);

/* 
  Leonardo pro micro
  2 SDA
  3 SCL
 */

int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

byte custChar_1[] = {
  B11011,
  B10101,
  B10101,
  B10001,
  B10001,
  B10101,
  B10101,
  B11011
};

byte custChar_2[] = {
  B11011,
  B10101,
  B10001,
  B10101,
  B10001,
  B10001,
  B10101,
  B11011
};


void setup() {

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, custChar_1);
  lcd.createChar(1, custChar_2);
  lcd.home();
}


void loop() {
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 0);
    lcd.printByte(0);
    lcd.setCursor(i, 1);
    lcd.printByte(1);
    delay(100);
  }


  delay(1000);
  lcd.clear();
  delay(1000);
}
