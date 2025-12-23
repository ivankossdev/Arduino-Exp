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

void charInit();

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  for(int i = 0; i < 100; i++){
    lcd.setCursor(0, 0);
    lcd.print(i); 
    delay(500);
  }
  lcd.clear();
}

