#include "display.h"

void setup() {
  displayInit();
}

void loop() {
  NextRowString(true, (char *)"Test Display exapmle!!!");
  delay(2500);
  displayClear();
  delay(1000);
}
