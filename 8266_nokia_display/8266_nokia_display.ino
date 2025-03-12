#include "display.h"
#include "ESP8266WiFi.h"
#include <cstring>

void setup() {
  displayInit();
  for(int i = 0; i < 2; i++) Serial.printf("\n");
  // testShow((char *)"1--12345678 \n2--12345678 3--12345678");
}

void loop() {
  
  // testShow((char *)"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum pulvinar, tortor sit amet gravida mollis, mi lorem venenatis nunc, at auctor purus velit sed mauris. Suspendisse id sagittis massa, suscipit dignissim massa. Class aptent taciti leo.");
  testShow((char *)"1--12345678 2--12345678 3--12345678 4--12345678 5--12345678 6--12345678 7--12345678 8--12345678 ");
  testShow((char *)"1--12345678_\n2--12345678_3--12345678");
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

void testShow(char *str) {
  NextRowString(str);
  delay(2000);
  mylcd.LCDClear(0x00);
  delay(1000);
}