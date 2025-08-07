#include "display.h"

void setup() {
  displayInit(&display);
}

void loop() {
  
  for (int item = 2; item < 127; item++) {
    progressBar(&display, item, Up);
    showTotal(&display, item);
  }

  delay(500);

  for (int item = 126; item >= 2; item--) {
    progressBar(&display, item, Down);
    showTotal(&display, item);
  }

  clearProgressBar(&display);
  clearTotal(&display);
  delay(1000);
}
