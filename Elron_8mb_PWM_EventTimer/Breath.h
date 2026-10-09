#ifndef BREATH_H
#define BREATH_H

#include <Arduino.h>

// --- Жизненный цикл ---
void breathBegin();    // Вызвать один раз в setup()
void breathUpdate();   // Вызывать в loop() как можно чаще

// --- Управление на лету ---
void    breathSetSpeed(uint8_t speed);     // 0..100 (обрезается по границам)
uint8_t breathGetSpeed();                  // текущее значение

void breathStart();
void breathStop();
bool breathIsRunning();

#endif // BREATH_H