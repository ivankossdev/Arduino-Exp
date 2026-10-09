#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <Arduino.h>

// Инициализация модуля. Вызвать один раз в setup().
void heartbeatBegin();

// Выводим событие
void heartbeatUpdate(); 

#endif // HEARTBEAT_H