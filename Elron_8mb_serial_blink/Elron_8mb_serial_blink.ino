#include "EventTimer.h"


const unsigned long BLINK_INTERVAL = 1000;

EventTimer timer(BLINK_INTERVAL, 10);

void handlerLed(bool newState, int currentCount) {
  digitalWrite(LED_BUILTIN, newState ? HIGH : LOW);

  Serial.print("Count ");
  Serial.println(currentCount);

  if (currentCount == 9) {
    Serial.println();  // Отступ после 10 итераций
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  timer.setCallback(handlerLed);
}

void loop() {
  timer.tick();
}