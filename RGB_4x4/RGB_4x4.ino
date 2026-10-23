#include "matr.h"
#include "EventTimer.h"

static EventTimer stepTimer(1000);

void setup() {
  Serial.begin(9600);
  matrInit(3);
  stepTimer.setCallback(dotStep);   // dotWalk — это void(*)(), подходит
}

void loop() {
  stepTimer.tick();
}