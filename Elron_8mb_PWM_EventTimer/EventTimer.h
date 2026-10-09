#ifndef EVENT_TIMER_H
#define EVENT_TIMER_H

#include <Arduino.h>

// Колбэк на каждое срабатывание таймера.
//   state — новое состояние пина (после инверсии)
//   count — номер завершённого цикла, 0-based (0 .. maxIterations-1)
typedef void (*StateChangeCallback)(bool state);

class EventTimer {
public:
    // intervalMs    — период срабатывания, мс
    // maxIterations — количество тиков в одной серии (>= 1)
    // По умолчанию таймер запущен сразу после создания.
    EventTimer(unsigned long intervalMs);

    // --- Настройка ---
    void setCallback(StateChangeCallback cb);
    void setInterval(unsigned long intervalMs);

    // --- Управление ---
    void start();                          // сбросить сетку и запустить
    void stop();                           // остановить, сохранив состояние
    void reset();                          // вернуть всё в исходное

    // --- Основной вызов: дергать из loop() ---
    void tick();

    // --- Чтение состояния ---
    bool isRunning() const { return running; }
    bool getState()  const { return state; }
    unsigned long getInterval() const { return interval; }

private:
    unsigned long previousMillis;
    unsigned long interval;
    bool state;
    bool running;

    StateChangeCallback   onStateChange;
};

#endif // EVENT_TIMER_H