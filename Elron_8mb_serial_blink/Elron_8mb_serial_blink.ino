#include "EventTimer.h"

// --- Параметры задач ---
const unsigned long BLINK_INTERVAL = 1000;
const int           BLINK_COUNT    = 10;
const unsigned long TICK_INTERVAL  = 250;

// --- Таймеры ---
EventTimer blinkTimer(BLINK_INTERVAL, BLINK_COUNT);  // мигание + счёт
EventTimer tickTimer (TICK_INTERVAL, 1);             // «тик» — каждая серия из одного вызова

// --- Колбэки ---

// Мигание: раз в секунду
void onBlink(bool state, int count) {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
  Serial.print("Count ");
  Serial.println(count);
}

// Конец серии мигания: пустая строка
void onBlinkSeriesComplete() {
  Serial.println();
}

// «Тик»: раз в 250 мс
void onTick(bool state, int count) {
  (void)state;   // не используется
  (void)count;   // не используется
  Serial.print(".");
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // Мигание
  blinkTimer.setCallback(onBlink);
  blinkTimer.setSeriesCompleteCallback(onBlinkSeriesComplete);

  // Тик
  tickTimer.setCallback(onTick);

  // Оба таймера уже запущены конструктором — start() не нужен
}

void loop() {
  blinkTimer.tick();
  tickTimer.tick();
}