#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "MyMillis.h"

char data[32] = { '\0' };
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
MyMillis interval_1(250);
MyMillis interval_2(1000);
MyMillis scroll(500);
int count_1 = 0;
int count_2 = 0;

int pos = 0; 
String message = "Hello led!!!"; 

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
  scroll.tic(scrollText, 0, "Hello led!!!", 5); 
}

void scrollText(int row, String message, int lcdColumns) {
  if(pos >= message.length()){
    pos = 0; 
    // for (int i = 0; i < lcdColumns; i++) {
    //   message = " " + message;
    // }
  }
  message = message + " ";
  lcd.setCursor(11, row);
  lcd.print(message.substring(pos, pos + lcdColumns));
  pos++;
}
