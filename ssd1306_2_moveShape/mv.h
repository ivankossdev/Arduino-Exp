#ifndef _MV_H
#define _MV_H
#include "Arduino.h"
#include "drawShape.h"
#include "config.h"

// Вспомогательные функции для стен
void constrainToWalls(Shape& s);
void resolveCollision(Shape& a, Shape& b);   // вместо checkCollisionBetween

// Основная логика
void Logic_1();
void drawFrame();

// Функции для работы с позиционированием (если используются)
int16_t MAX_XPOS(Shape& mShape);
int16_t MAX_YPOS(Shape& mShape);
void stepTowardsTarget(Shape& mShape);

extern DrawShape drawShape;
extern Shape shapes[MAX_SHAPES];
extern int shapeCount;

#endif