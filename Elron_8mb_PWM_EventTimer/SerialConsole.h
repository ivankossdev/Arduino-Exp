#ifndef SERIAL_CONSOLE_H
#define SERIAL_CONSOLE_H

#include <Arduino.h>

// Инициализация модуля. Вызвать один раз в setup().
void consoleBegin();

// Обновление: чтение Serial и обработка строк. Вызывать в loop().
// Функция неблокирующая — читает только то, что уже пришло.
void consoleUpdate();

#endif // SERIAL_CONSOLE_H