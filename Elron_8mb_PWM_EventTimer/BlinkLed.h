#ifndef BLINK_LED_H
#define BLINK_LED_H

#include <Arduino.h>

// Инициализация модуля. Вызвать один раз в setup().
void blinkBegin();

// Обновление состояния. Вызывать в loop() как можно чаще.
// Функция неблокирующая — возвращает управление сразу.
void blinkUpdate();

#endif // BLINK_LED_H