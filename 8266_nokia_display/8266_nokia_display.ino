#include "display.h"
#include "ESP8266WiFi.h"

void setup() {
  displayInit();
}

void loop() {
  printNextString((char *)"Lorem \nipsum \ndolor.");
  delay(5000);
  mylcd.LCDClear(0x00);
  delay(1000);
}
