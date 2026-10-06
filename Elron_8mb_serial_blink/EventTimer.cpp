#include "EventTimer.h"

EventTimer::EventTimer(unsigned long intervalMs, int maxIterations)
    : previousMillis(millis()),
      interval(intervalMs == 0 ? 1 : intervalMs),   // защита от 0
      state(false),
      count(0),
      maxIterations(maxIterations < 1 ? 1 : maxIterations),
      running(true),
      oneShot(false),
      onStateChange(nullptr),
      onSeriesComplete(nullptr) {}

void EventTimer::setCallback(StateChangeCallback cb) {
    onStateChange = cb;
}

void EventTimer::setSeriesCompleteCallback(SeriesCompleteCallback cb) {
    onSeriesComplete = cb;
}

void EventTimer::setInterval(unsigned long intervalMs) {
    interval = (intervalMs == 0) ? 1 : intervalMs;
    previousMillis = millis();   // перезапустить сетку с новым периодом
}

void EventTimer::setMaxIterations(int n) {
    maxIterations = (n < 1) ? 1 : n;
    if (count >= maxIterations) count = 0;
}

void EventTimer::setOneShot(bool enable) {
    oneShot = enable;
}

void EventTimer::start() {
    running = true;
    previousMillis = millis();   // сетка стартует «сейчас»
}

void EventTimer::stop() {
    running = false;
}

void EventTimer::reset() {
    state = false;
    count = 0;
    previousMillis = millis();
}

void EventTimer::tick() {
    if (!running) return;

    unsigned long now = millis();
    if (now - previousMillis < interval) return;

    // Сдвигаем сетку на полный период, а не на текущее время,
    // чтобы не накапливать дрейф от задержек loop().
    previousMillis += interval;

    // Если мы отстали больше чем на один период — ресинхронизируемся,
    // чтобы не выдать пачку срабатываний подряд.
    if (now - previousMillis >= interval) {
        previousMillis = now;
    }

    // Переход состояния
    state = !state;

    // Колбэк с новым state и индексом завершённого цикла (0-based)
    if (onStateChange != nullptr) {
        onStateChange(state, count);
    }

    count++;

    // Завершение серии
    if (count >= maxIterations) {
        if (onSeriesComplete != nullptr) {
            onSeriesComplete();
        }
        count = 0;

        if (oneShot) {
            running = false;
        }
    }
}