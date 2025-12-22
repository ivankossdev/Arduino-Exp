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
// char 1
byte custChar_1_0[] = { B00001, B00001, B00001, B01111, B01111, B01111, B00001, B00001 };
byte custChar_1_1[] = { B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000 };
byte custChar_1_2[] = { B00001, B00001, B00001, B00001, B00001, B01111, B01111, B01111 };
byte custChar_1_3[] = { B11000, B11000, B11000, B11000, B11000, B11111, B11111, B11111 };
//char 2
byte custChar_2_0[] = { B00111, B01111, B11100, B11100, B11100, B00000, B00000, B00000 };
byte custChar_2_1[] = { B11100, B11110, B00111, B00111, B00111, B01110, B11100, B11000 };
byte custChar_2_2[] = { B00011, B00111, B01110, B11100, B11100, B11111, B11111, B11111 };
byte custChar_2_3[] = { B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111 };

/*
  0     1
  00011 11000
  00011 11000
  11100 00111
  11100 00111
  11100 00111
  00000 01110
  00000 11100
  00000 11000
  2     3   
  00011 10000
  00111 00000
  01110 00000
  11100 00000
  11100 00000
  11111 11111
  11111 11111
  11111 11111

*/
void charInit();

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.clear();
  void charInit();
}


void loop() {
  // 1
  lcd.setCursor(0, 0);
  lcd.printByte(0);
  lcd.setCursor(1, 0);
  lcd.printByte(1);
  lcd.setCursor(0, 1);
  lcd.printByte(2);
  lcd.setCursor(1, 1);
  lcd.printByte(3);
  // 2 
  lcd.setCursor(2, 0);
  lcd.printByte(4);
  lcd.setCursor(3, 0);
  lcd.printByte(5);
  lcd.setCursor(2, 1);
  lcd.printByte(6);
  lcd.setCursor(3, 1);
  lcd.printByte(7);

  //String 
  lcd.setCursor(4, 1);
  lcd.print("34567890");
}

void charInit(){
  lcd.createChar(0, custChar_1_0);
  lcd.createChar(1, custChar_1_1);
  lcd.createChar(2, custChar_1_2);
  lcd.createChar(3, custChar_1_3);
  lcd.createChar(4, custChar_2_0);
  lcd.createChar(5, custChar_2_1);
  lcd.createChar(6, custChar_2_2);
  lcd.createChar(7, custChar_2_3);
  lcd.home();
}
