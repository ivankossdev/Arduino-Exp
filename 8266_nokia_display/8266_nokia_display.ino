#include "display.h"
#include "ESP8266WiFi.h"

void setup() {
  displayInit();
}

void loop() {
  NextRowString((char *)"adc123456789512\n123\n1234\n12345\n123456");
  delay(5000);
  mylcd.LCDClear(0x00);
  delay(1000);
  // drawStar();
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
