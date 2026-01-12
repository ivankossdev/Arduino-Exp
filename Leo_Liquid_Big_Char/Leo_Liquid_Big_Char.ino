#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DISPLAY_ADDR 0x27
LiquidCrystal_I2C lcd(DISPLAY_ADDR, 16, 2);

uint8_t LT[8] = { 0b00111, 0b01111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
uint8_t UB[8] = { 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 };
uint8_t RT[8] = { 0b11100, 0b11110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
uint8_t LL[8] = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b01111, 0b00111 };
uint8_t LB[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };
uint8_t LR[8] = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11110, 0b11100 };
uint8_t UMB[8] = { 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111 };
uint8_t LMB[8] = { 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };

void drawDig(byte dig, byte x, byte y) {
  switch (dig) {
    case 0:
      lcd.setCursor(x, y);  
      lcd.write(0);         
      lcd.write(1);         
      lcd.write(2);
      lcd.setCursor(x, y + 1);  
      lcd.write(3);             
      lcd.write(4);             
      lcd.write(5);
      break;
    case 1:
      lcd.setCursor(x + 1, y);
      lcd.write(1);
      lcd.write(2);
      lcd.setCursor(x + 2, y + 1);
      lcd.write(5);
      break;
    case 2:
      lcd.setCursor(x, y);
      lcd.write(6);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(7);
      break;
    case 3:
      lcd.setCursor(x, y);
      lcd.write(6);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(7);
      lcd.write(7);
      lcd.write(5);
      break;
    case 4:
      lcd.setCursor(x, y);
      lcd.write(3);
      lcd.write(4);
      lcd.write(2);
      lcd.setCursor(x + 2, y + 1);
      lcd.write(5);
      break;
    case 5:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(6);
      lcd.setCursor(x, y + 1);
      lcd.write(7);
      lcd.write(7);
      lcd.write(5);
      break;
    case 6:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(6);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(5);
      break;
    case 7:
      lcd.setCursor(x, y);
      lcd.write(1);
      lcd.write(1);
      lcd.write(2);
      lcd.setCursor(x + 1, y + 1);
      lcd.write(0);
      break;
    case 8:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(5);
      break;
    case 9:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x + 1, y + 1);
      lcd.write(4);
      lcd.write(5);
      break;
    case 10:
      lcd.setCursor(x, y);
      lcd.write(32);
      lcd.write(32);
      lcd.write(32);
      lcd.setCursor(x, y + 1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(32);
      break;
  }
}

void clearSegment(byte dig, byte x, byte y) {
  switch (dig) {
    case 0:
      lcd.setCursor(x, y);  
      lcd.print(' ');         
      lcd.print(' ');          
      lcd.print(' '); 
      lcd.setCursor(x, y + 1);  
      lcd.print(' ');              
      lcd.print(' ');             
      lcd.print(' '); 
      break;
    case 1:
      lcd.setCursor(x + 1, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x + 2, y + 1);
      lcd.print(' '); 
      break;
    case 2:
      lcd.setCursor(x, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x, y + 1);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      break;
    case 3:
      lcd.setCursor(x, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x, y + 1);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      break;
    case 4:
      lcd.setCursor(x, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x + 2, y + 1);
      lcd.print(' '); 
      break;
    case 5:
      lcd.setCursor(x, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x, y + 1);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      break;
    case 6:
      lcd.setCursor(x, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x, y + 1);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      break;
    case 7:
      lcd.setCursor(x, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x + 1, y + 1);
      lcd.print(' '); 
      break;
    case 8:
      lcd.setCursor(x, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x, y + 1);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      break;
    case 9:
      lcd.setCursor(x, y);
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.print(' '); 
      lcd.setCursor(x + 1, y + 1);
      lcd.print(' '); 
      lcd.print(' '); 
      break;
    case 10:
      lcd.setCursor(x, y);
      lcd.write(32);
      lcd.write(32);
      lcd.write(32);
      lcd.setCursor(x, y + 1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(32);
      break;
  }
}
// uint8_t segment[4] = {'\0'}; 

void drawClock(uint8_t digital, byte x = 0, byte y = 0) {
  
  // lcd.clear();
  drawDig(1, x, y);
  drawDig(2, x + 4, y);
  drawDig(3, x + 8, y);
  drawDig(4, x + 12, y);
}

void customChar() {
  lcd.createChar(0, LT);
  lcd.createChar(1, UB);
  lcd.createChar(2, RT);
  lcd.createChar(3, LL);
  lcd.createChar(4, LB);
  lcd.createChar(5, LR);
  lcd.createChar(6, UMB);
  lcd.createChar(7, LMB);
}


void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  customChar();
}

void loop() {
  drawClock(1, 0, 0);
  // delay(500);
  clearSegment(10, 0, 0);
  // delay(500);
}
