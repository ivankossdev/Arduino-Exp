#include "matr.h"

void setup() {
  Serial.begin(9600);
  matrInit(3); // 3 яркость свечения 0 - 255 
}

void loop() {
  dotWalk();
}