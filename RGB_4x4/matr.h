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

/* Тик визуальных эффектов: гасит вспышки STANDSTILL и GOAL,
 * когда их время вышло. Вызывать часто (раз в 5–20 мс). */
void dotVisualTick();

/* Залить всю матрицу цветом калибровки (COLOR_CALIB_*).
 * Используется как «заставка» на время recalibrate(). */
void matrShowCalibrating();

/* Перерисовать текущий кадр (точка + цель + активные вспышки).
 * Нужна после matrShowCalibrating(), чтобы вернуть игровую картинку. */
void matrRefresh();

const char* directionName(DirectionOfMovement d);