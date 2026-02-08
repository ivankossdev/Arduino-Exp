#include <Arduino.h>
#include "serialSet.h"

SerialSet serialSet(9600);
int ledPin = 9;

void setup() {
  serialSet.Init();
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  serialSet.SetCMD();
  for (int brightness = 0; brightness <= 255; brightness++) {
    analogWrite(ledPin, brightness);
    delay(10);
  }
  for (int brightness = 255; brightness >= 0; brightness--) {
    analogWrite(ledPin, brightness);
    delay(10);
  }
}