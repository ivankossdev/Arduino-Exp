#ifndef _MV_H
#define _MV_H

#include "Arduino.h"
#include "drawShape.h"
#include "config.h"
#include "Wall.h"

// Вспомогательные функции для стен
void constrainToWalls(Shape& s);
void resolveCollision(Shape& a, Shape& b);   
void checkShapeWallCollision(Shape& s, const Wall& wall);

// Основная логика
void Logic_1();
void drawFrame();

extern DrawShape drawShape;
extern Shape shapes[MAX_SHAPES];
extern Wall walls[MAX_WALLS];
extern int shapeCount;
extern int wallCount;

void predictAndResolveCollisions();

#endif
