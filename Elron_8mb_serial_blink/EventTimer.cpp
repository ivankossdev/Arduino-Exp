#include "EventTimer.h"

EventTimer::EventTimer(unsigned long intervalMs, int maxIterations) 
    : previousMillis(0), interval(intervalMs), state(false), count(0), maxCount(maxIterations), onStateChange(nullptr) {}

void EventTimer::setCallback(StateChangeCallback callback) {
    onStateChange = callback;
}

void EventTimer::tick() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        state = !state;

        if (onStateChange != nullptr) {
            onStateChange(state, count);
        }

        count++;
        if (count >= maxCount) {
            count = 0;
        }
    }
}