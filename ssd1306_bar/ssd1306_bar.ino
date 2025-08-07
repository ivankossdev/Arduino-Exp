#include "display.h"

void setup() {
  displayInit(&display);
}

void loop() {
  
  for (int item = 2; item < 127; item++) {
    progressBar(&display, item, up);
    showTotal(&display, item);
  }
  
  delay(500);

  for (int item = 126; item >= 2; item--) {
    progressBar(&display, item, down);
    showTotal(&display, item);
  }

  clearProgressBar(&display);
  clearTotal(&display);
  delay(2000);

  progressBarSet(&display, 100);
  showTotal(&display, 100);
  delay(2000);

  clearProgressBar(&display);
  clearTotal(&display);
  
  progressBarSet(&display, 10);
  showTotal(&display, 10);
  delay(2000);

  clearProgressBar(&display);
  clearTotal(&display);
  delay(2000);

}
