#include "EventTimer.h"

// ---- Конструкторы ----

EventTimer::EventTimer()
    : previousMillis(0),
      interval(1),
      running(true),
      onTick(nullptr) {}

EventTimer::EventTimer(unsigned long intervalMs)
    : previousMillis(0),
      interval(intervalMs == 0 ? 1 : intervalMs),   // защита от 0
      running(true),
      onTick(nullptr) {}

// ---- Настройка ----

void EventTimer::setCallback(TimerCallback cb) {
    onTick = cb;
}

void EventTimer::setInterval(unsigned long intervalMs) {
    interval = (intervalMs == 0) ? 1 : intervalMs;
    previousMillis = millis();   // перезапустить сетку с новым периодом
}

// ---- Управление ----

void EventTimer::start() {
    running = true;
    previousMillis = millis();   // сетка стартует «сейчас»
}

void EventTimer::stop() {
    running = false;
}

// ---- Тик ----

void EventTimer::tick() {
    if (!running) return;

    unsigned long now = millis();
    if (now - previousMillis < interval) return;

    // Сдвигаем сетку на полный период, а не на текущее время,
    // чтобы не накапливать дрейф от задержек loop().
    previousMillis += interval;

    // Если отстали больше чем на период — ресинхронизируемся,
    // чтобы не выдать пачку срабатываний подряд.
    if (now - previousMillis >= interval) {
        previousMillis = now;
    }

    if (onTick != nullptr) {
        onTick();
    }
}