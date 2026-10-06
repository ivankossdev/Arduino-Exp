#ifndef EVENT_TIMER_H
#define EVENT_TIMER_H

#include <Arduino.h>

// Тип функции-колбэка: принимает состояние и текущий счетчик
typedef void (*StateChangeCallback)(bool state, int count);

class EventTimer {
private:
    unsigned long previousMillis;
    unsigned long interval;
    bool state;
    int count;
    int maxCount;
    StateChangeCallback onStateChange;

public:
    EventTimer(unsigned long intervalMs, int maxIterations = 10);
    
    void setCallback(StateChangeCallback callback);
    void tick();
};

#endif // EVENT_TIMER_H