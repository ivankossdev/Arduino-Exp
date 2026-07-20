#ifndef _MV_H
#define _MV_H

#include "Arduino.h"
#include "drawShape.h"
#include "config.h"

// Класс стены/препятствия: просто прямоугольник с координатами
class Wall {
public:
  int16_t x;
  int16_t y;
  int16_t w; // ширина
  int16_t h; // высота

  Wall(int16_t _x, int16_t _y, int16_t _w, int16_t _h)
    : x(_x), y(_y), w(_w), h(_h) {}
    
  Wall() = default;
};

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
