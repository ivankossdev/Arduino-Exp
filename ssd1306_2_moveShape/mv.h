#ifndef _MV_H
#define _MV_H

#include "Arduino.h"
#include "drawShape.h"
#include "config.h"
#include "Wall.h"

void constrainToWalls(Shape& s);
void resolveCollision(Shape& a, Shape& b);
bool checkShapeWallCollision(Shape& s, const Wall& wall);  // теперь возвращает bool

void Logic_1();
void drawFrame();

extern DrawShape drawShape;
extern Shape shapes[MAX_SHAPES];
extern Wall walls[MAX_WALLS];
extern int shapeCount;
extern int wallCount;

void predictAndResolveCollisions();

#endif