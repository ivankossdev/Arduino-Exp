#include "EventTimer.h"

const unsigned long BLINK_INTERVAL = 500;
const int           BLINK_COUNT    = 10;

EventTimer blinkTimer(BLINK_INTERVAL, BLINK_COUNT);

// Вызывается на каждом тике: новое состояние + номер цикла 0..9
void onBlink(bool state, int count) {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
  Serial.print("Count ");
  Serial.println(count);
}

// Вызывается один раз между сериями — раньше это был хардкод "== 9"
void onBlinkSeriesComplete() {
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  blinkTimer.setCallback(onBlink);
  blinkTimer.setSeriesCompleteCallback(onBlinkSeriesComplete);
  // blinkTimer уже запущен конструктором; start() не обязателен
}

void loop() {
  blinkTimer.tick();
}