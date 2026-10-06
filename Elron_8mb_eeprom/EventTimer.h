#ifndef EVENT_TIMER_H
#define EVENT_TIMER_H

#include <Arduino.h>

// Колбэк на каждое срабатывание таймера.
//   state — новое состояние пина (после инверсии)
//   count — номер завершённого цикла, 0-based (0 .. maxIterations-1)
typedef void (*StateChangeCallback)(bool state, int count);

// Колбэк на завершение серии (вызывается после последнего тика,
// перед сбросом счётчика; удобно для отступов в Serial, смены режима и т.п.)
typedef void (*SeriesCompleteCallback)();

class EventTimer {
public:
    // intervalMs    — период срабатывания, мс
    // maxIterations — количество тиков в одной серии (>= 1)
    // По умолчанию таймер запущен сразу после создания.
    EventTimer(unsigned long intervalMs, int maxIterations = 10);

    // --- Настройка ---
    void setCallback(StateChangeCallback cb);
    void setSeriesCompleteCallback(SeriesCompleteCallback cb);
    void setInterval(unsigned long intervalMs);
    void setMaxIterations(int n);
    void setOneShot(bool enable);          // остановиться после первой серии

    // --- Управление ---
    void start();                          // сбросить сетку и запустить
    void stop();                           // остановить, сохранив состояние
    void reset();                          // вернуть всё в исходное

    // --- Основной вызов: дергать из loop() ---
    void tick();

    // --- Чтение состояния ---
    bool isRunning() const { return running; }
    bool getState()  const { return state; }
    int  getCount()  const { return count; }
    unsigned long getInterval() const { return interval; }

private:
    unsigned long previousMillis;
    unsigned long interval;
    bool state;
    int  count;
    int  maxIterations;
    bool running;
    bool oneShot;

    StateChangeCallback   onStateChange;
    SeriesCompleteCallback onSeriesComplete;
};

#endif // EVENT_TIMER_H