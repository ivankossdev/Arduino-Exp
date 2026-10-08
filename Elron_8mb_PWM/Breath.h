#ifndef BREATH_H
#define BREATH_H

#include <Arduino.h>

// Инициализация модуля. Вызвать один раз в setup().
void breathBegin();

// Обновление состояния. Вызывать в loop() как можно чаще.
// Функция неблокирующая — возвращает управление сразу.
void breathUpdate();

#endif // BREATH_H