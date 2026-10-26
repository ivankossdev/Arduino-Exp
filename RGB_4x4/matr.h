#pragma once
#include <Arduino.h>

enum DirectionOfMovement {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  STANDSTILL,   // точка не сдвинулась: from == to
  JUMP,         // сдвинулась, но не на соседнюю клетку
  FIRST         // предыдущей позиции ещё не было (from < 0)
};

/* Информация об одном шаге точки. Возвращается из dotStep(). */
struct DotState {
  int                 pos;
  int                 row;
  int                 col;
  DirectionOfMovement dir;
};

void matrInit(uint8_t br);

/* Один шаг точки по пути. Обновляет матрицу и возвращает,
 * куда и как сдвинулась точка. Про Serial, millis и таймеры не знает. */
DotState dotStep();

/* Строковое имя направления (для логов/отладки). */
const char* directionName(DirectionOfMovement d);