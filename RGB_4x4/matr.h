#pragma once
#include <Arduino.h>

enum DirectionOfMovement {
  UP, DOWN, LEFT, RIGHT,
  STANDSTILL, JUMP, FIRST
};

struct DotState {
  int                 pos;
  int                 row;
  int                 col;
  DirectionOfMovement dir;
};

void matrInit(uint8_t br);

/* Установить точку в стартовую позицию и отрисовать её. */
DotState dotInit(int startPos);

/* Авто-шаг: случайный сосед (рандомное блуждание). */
DotState dotStep();

/* Ручной шаг: сдвинуть точку на одну клетку в заданном направлении.
 * Если сдвиг невозможен (край матрицы) — точка остаётся, dir = STANDSTILL. */
DotState dotMoveDirection(DirectionOfMovement dir);

const char* directionName(DirectionOfMovement d);