#include "display.h"

void setup() {
  displayInit(&display);
}

void loop() {
  progressBar(&display);
  delay(1000);
  clearProgressBar(&display);
  delay(1000);
}


