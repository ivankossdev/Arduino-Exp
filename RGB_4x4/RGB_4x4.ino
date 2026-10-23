#include "matr.h"

void setup() {
  Serial.begin(9600);
  matrInit();
}

void loop() {
  dotWalk();
}