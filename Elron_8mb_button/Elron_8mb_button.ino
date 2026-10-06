#include "EventTimer.h"

EventTimer blinkTimer(1000, 10);
volatile bool buttonPressed = false;
bool fastMode = false;

void onBlink(bool state, int count) {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
  Serial.print(fastMode ? "FAST " : "SLOW ");
  Serial.print("Count ");
  Serial.println(count);
}

void onSeriesComplete() {
  Serial.println();
}

void onButtonISR() {
  buttonPressed = true;
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BTN_BUILTIN, INPUT_PULLUP);

  blinkTimer.setCallback(onBlink);
  blinkTimer.setSeriesCompleteCallback(onSeriesComplete);

  attachInterrupt(digitalPinToInterrupt(BTN_BUILTIN), onButtonISR, FALLING);
}

void loop() {
  blinkTimer.tick();

  if (buttonPressed) {
    buttonPressed = false;

    // антидребезг
    static unsigned long lastPress = 0;
    if (millis() - lastPress > 200) {
      lastPress = millis();

      fastMode = !fastMode;
      blinkTimer.setInterval(fastMode ? 250 : 1000);
      Serial.print("Mode -> ");
      Serial.println(fastMode ? "FAST" : "SLOW");
    }
  }
}