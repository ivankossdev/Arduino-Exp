#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "I2C_Device.h"

/* 
  Leonardo pro micro
  2 SDA
  3 SCL
 */

int lcdColumns = 16;
int lcdRows = 2;
String devices = "";

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
I2C_Device search;
String messageToScroll = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";

void setup() {

  lcd.init();
  lcd.backlight();
  devices = search.searchDevice();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Display test");
  scrollText(1, devices, 300, lcdColumns);
  lcd.clear();
}


void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Scroll string");
  scrollText(1, messageToScroll, 350, lcdColumns);
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
