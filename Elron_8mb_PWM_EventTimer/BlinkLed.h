#ifndef BLINK_LED_H
#define BLINK_LED_H

#include <Arduino.h>

// --- Жизненный цикл ---
void blinkBegin();     // Вызвать один раз в setup()
void blinkUpdate();    // Вызывать в loop() как можно чаще

// --- Управление на лету ---
void          blinkSetInterval(unsigned long intervalMs);   // минимум 1 мс
unsigned long blinkGetInterval();

void blinkStart();
void blinkStop();
bool blinkIsRunning();

#endif // BLINK_LED_H