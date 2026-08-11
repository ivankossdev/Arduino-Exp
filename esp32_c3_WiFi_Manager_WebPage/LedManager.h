#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Arduino.h>

enum LedMode {
    LED_OFF,
    LED_ON,
    LED_BLINK_SLOW,     // 1 Гц (500ms вкл/выкл)
    LED_BLINK_FAST,     // 5 Гц (100ms вкл/выкл)
    LED_BLINK_ERROR     // короткие вспышки (200ms вкл, 800ms выкл)
};

class LedManager {
public:
    LedManager();
    bool begin(int pin, bool activeLow = true); // по умолчанию LOW = включён
    void update();              // вызывать в loop() для обновления мигания
    void setMode(LedMode mode);
    void on();                  // быстро включить
    void off();                 // быстро выключить
    void toggle();              // переключить состояние (для ручного управления)
    bool getState() const;

private:
    int _pin;
    bool _activeLow;            // true = LOW включает светодиод
    bool _state;                // текущее логическое состояние (true = включён)
    LedMode _mode;
    unsigned long _lastToggle;
    unsigned long _interval;
    bool _ledOn;                // фактическое состояние вывода

    void _setOutput(bool on);   // устанавливает вывод с учётом activeLow
    void _updateInterval();     // вычисляет интервал для текущего режима
};

#endif