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
// 
byte custChar_1_0[] = { B00001, B00001, B00001, B01111, B01111, B01111, B00001, B00001 };
byte custChar_1_1[] = { B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000 };
byte custChar_1_2[] = { B00001, B00001, B00001, B00001, B00001, B01111, B01111, B01111 };
byte custChar_1_3[] = { B11000, B11000, B11000, B11000, B11000, B11111, B11111, B11111 };

byte custChar_2_0[] = {};
byte custChar_2_1[] = {};
byte custChar_2_2[] = {};
byte custChar_2_3[] = {};

/*
  1_0   1_1
  00011 11000
  00011 11000
  11100 00111
  11100 00111
  11100 00111
  00000 01110
  00000 11100
  00001 11000
  1_2     1_3   
  00011 10000
  00111 00000
  01110 00000
  11100 00000
  11100 00000
  11111 11111
  11111 11111
  11111 11111

*/


void setup() {

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, custChar_1_0);
  lcd.createChar(1, custChar_1_1);
  lcd.createChar(2, custChar_1_2);
  lcd.createChar(3, custChar_1_3);
  lcd.home();
}


void loop() {
  lcd.setCursor(0, 0);
  lcd.printByte(0);
  lcd.setCursor(1, 0);
  lcd.printByte(1);
  lcd.setCursor(0, 1);
  lcd.printByte(2);
  lcd.setCursor(1, 1);
  lcd.printByte(3);
  lcd.setCursor(2, 0);
  lcd.print("2345678901");
  delay(10000);
  lcd.clear();
  delay(1000);
}
