#ifndef _MV_H
#define _MV_H

#include "Arduino.h"
#include "DrawShape.h"
#include "config.h"
#include "Shape.h"
#include "Wall.h"

// Функции
void Logic_1();
void drawFrame();
void constrainToWalls(Shape& s);
void resolveCollision(Shape& a, Shape& b);
void checkShapeWallCollision(Shape& s, const Wall& wall);
void predictAndResolveCollisions();

// Глобальные переменные
extern DrawShape drawShape;
extern Shape shapes[MAX_SHAPES];
extern Wall walls[MAX_WALLS];
extern int shapeCount;
extern int wallCount;

#endif
