#include "EventTimer.h"

unsigned long previousBlink = 0;
unsigned long previousTick  = 0;
const unsigned long BLINK_INTERVAL = 1000;
const unsigned long TICK_INTERVAL  = 250;

bool ledLevel = false;
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  // Задача 1: мигание + счёт
  if (now - previousBlink >= BLINK_INTERVAL) {
    previousBlink = now;
    ledLevel = !ledLevel;
    digitalWrite(LED_BUILTIN, ledLevel ? HIGH : LOW);

    Serial.print("Count ");
    Serial.println(count);

    if (++count >= 10) {
      count = 0;
      Serial.println();
    }
  }

  // Задача 2: «тик» каждые 250 мс — не мешает миганию
  if (now - previousTick >= TICK_INTERVAL) {
    previousTick = now;
    Serial.print(".");          // точки сыплются в 4 раза чаще, чем Count
  }
}