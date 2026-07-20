#ifndef _MV_H
#define _MV_H
#include "Arduino.h"
#include "drawShape.h"
#include "config.h"

// Вспомогательные функции для стен
void constrainToWalls(Shape& s);
void resolveCollision(Shape& a, Shape& b);   

// Основная логика
void Logic_1();
void drawFrame();

extern DrawShape drawShape;
extern Shape shapes[MAX_SHAPES];
extern int shapeCount;

void predictAndResolveCollisions();

#endif