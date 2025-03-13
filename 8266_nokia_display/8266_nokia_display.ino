#include "display.h"
#include "ESP8266WiFi.h"
#include <cstring>

void setup() {
  displayInit();
}

void loop() {
  
  testShow(false, (char *)"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum pulvinar, tortor sit amet gravida mollis, mi lorem venenatis nunc, at auctor purus velit sed mauris. Suspendisse id sagittis massa, suscipit dignissim massa. Class aptent taciti leo.");
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

void testShow(bool delay_, char *str) {
  NextRowString(delay_,str);
  delay(2000);
  mylcd.LCDClear(0x00);
  delay(1000);
}