#include "display.h"

void setup() {
  displayInit(&display);
}



void loop() {
  int mvX = 0;
  for (int item = 0; item < 127; item++) {
    progressBar(&display, item);
    showTotal(&display, item);
    moveShape(&display, mvX);
    if (item < 63) {
      mvX++;
    } else {
      mvX--;
    }
  }

  delay(1000);

  clearProgressBar(&display);
  clearTotal(&display);
  delay(1000);
}
