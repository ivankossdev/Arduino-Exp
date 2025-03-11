#include "display.h"
#include "ESP8266WiFi.h"
#include <cstring>

void setup() {
  displayInit();
}

void loop() {
  testShow((char *)"abc123456789abc123456789abc123456789abc123456789abc123456789abc123456789zxc987654321");
  drawStar();
}

void drawStar() {
  for (int i = 0; i < 12; i++) {
    for (int y = 0; y < 6; y++) {
      mylcd.LCDgotoXY(i * 7, y);
      mylcd.LCDCharacter('*');
    }
    delay(100);
  }
  
  delay(5000);
  mylcd.LCDClear(0x00);
  delay(1000);
}

void testShow(char *str) {
  NextRowString(str);
  delay(5000);
  mylcd.LCDClear(0x00);
  delay(1000);
}