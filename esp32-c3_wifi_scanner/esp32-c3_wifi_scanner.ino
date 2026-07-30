#include "wifi_scanner.h"

WiFiScanner scanner;
const unsigned long WIFI_SCANNER_MS = 30000;
unsigned long previousTime = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {

  unsigned long now = millis();
  if (now - previousTime >= WIFI_SCANNER_MS) {
    scanner.scan(); 
    previousTime = now;
  }
}
