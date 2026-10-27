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
  bool                reachedTarget; 
};

void matrInit(uint8_t br);
DotState dotInit(int startPos);
DotState dotStep();
DotState dotMoveDirection(DirectionOfMovement dir);

/* Тик визуальных эффектов: гасит вспышку после STANDSTILL,
 * когда её время вышло. Вызывать часто (раз в 5–20 мс). */
void dotVisualTick();

const char* directionName(DirectionOfMovement d);