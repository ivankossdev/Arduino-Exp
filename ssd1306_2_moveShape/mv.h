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

extern DrawShape drawShape;
extern Shape shapes[MAX_SHAPES];
extern int shapeCount;

// ... существующий код ...

extern unsigned long lastMovementTime;
extern const unsigned long STUCK_TIMEOUT;
extern int16_t prevPos[MAX_SHAPES][2];

#endif