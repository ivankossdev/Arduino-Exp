
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "I2C_Device.h"

/* 
  Leonardo pro micro
  2 SDA
  3 SCL
 */

LiquidCrystal_I2C lcd(0x27, 16, 2);
I2C_Device search;

void setup() {
  lcd.init();
  
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(search.searchDevice());
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Display test");
}


void loop() {
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i = 0; i < lcdColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}
