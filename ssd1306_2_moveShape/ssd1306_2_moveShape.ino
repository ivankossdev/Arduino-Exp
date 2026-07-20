#include "mv.h"

void setup() {
  drawShape.init();
  randomSeed(analogRead(0));
}

void loop() {
  Logic_1();
  drawFrame();
}