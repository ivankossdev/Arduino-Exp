#include "display.h"

void setup() {
  displayInit(&display);
}

void loop() {
  for (int item = 0; item < 127; item++) {
    progressBar(&display, item);
    showTotal(&display, item);
  }
  delay(1000);
  
  clearProgressBar(&display);
  clearTotal(&display);
  delay(1000);
}


