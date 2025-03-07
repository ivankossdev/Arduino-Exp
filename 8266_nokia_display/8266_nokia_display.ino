#include "display.h"
#include "ESP8266WiFi.h"

void setup() {
  displayInit();
}

void loop() {
  NextRowString((char *)"11111 11111\n22222 22222\n33333 33333\n44444 44444\n55555 55555\n66666 66666");
  delay(5000);
  mylcd.LCDClear(0x00);
  delay(1000);
  for (int i = 0; i < 12; i++) {
    for(int y = 0; y < 6; y++){
      mylcd.LCDgotoXY(i * 7, y);
      mylcd.LCDCharacter('*');
    }
    delay(100);
  }
  delay(5000);
  mylcd.LCDClear(0x00);
  delay(1000);
}
