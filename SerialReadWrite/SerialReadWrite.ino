#include <Arduino.h>
#include "serialSet.h"

SerialSet serialSet(9600);

void setup() {
  serialSet.Init();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  serialSet.SetCMD();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}