#include <Arduino.h>
#include "serialSet.h"

SerialSet serialSet(9600);

void setup() {
  serialSet.Init();
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
}

void loop() {
  serialSet.SetCMD();
}