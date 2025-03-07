#include "display.h"
#include "ESP8266WiFi.h"

void setup() {
  displayInit();
}

void loop() {
  NextRowString((char *)"12345\n09876\n67890\n12345\n98765");
  delay(5000);
  mylcd.LCDClear(0x00);
  delay(1000);
}
