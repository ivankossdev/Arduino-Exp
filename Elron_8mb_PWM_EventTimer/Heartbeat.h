#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <Arduino.h>

// --- Жизненный цикл ---
void heartbeatBegin();     // Вызвать один раз в setup()
void heartbeatUpdate();    // Вызывать в loop() как можно чаще

// --- Управление на лету ---
void          heartbeatSetInterval(unsigned long intervalMs);
unsigned long heartbeatGetInterval();

void heartbeatStart();
void heartbeatStop();
bool heartbeatIsRunning();

#endif // HEARTBEAT_H