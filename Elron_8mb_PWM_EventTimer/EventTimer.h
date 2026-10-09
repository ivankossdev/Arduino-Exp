#ifndef EVENT_TIMER_H
#define EVENT_TIMER_H

#include <Arduino.h>

// Колбэк, который таймер вызывает на каждом срабатывании.
// Никаких аргументов: таймер не знает, что делает потребитель —
// мигает он, дышит или шлёт пакет по Serial. Просто «тикает».
typedef void (*TimerCallback)();

class EventTimer {
public:
    // Интервал по умолчанию 1 мс, таймер сразу запущен.
    // Реальный интервал задать через setInterval().
    EventTimer();

    // Явный интервал, таймер сразу запущен.
    explicit EventTimer(unsigned long intervalMs);

    // --- Настройка ---
    void setCallback(TimerCallback cb);
    void setInterval(unsigned long intervalMs);   // перезапускает сетку

    // --- Управление ---
    void start();   // сбросить сетку и запустить
    void stop();    // остановить, интервал сохраняется

    // --- Основной вызов: дергать из loop() ---
    void tick();

    // --- Чтение ---
    bool          isRunning()   const { return running; }
    unsigned long getInterval() const { return interval; }

private:
    unsigned long previousMillis;
    unsigned long interval;
    bool          running;

    TimerCallback onTick;
};

#endif // EVENT_TIMER_H 