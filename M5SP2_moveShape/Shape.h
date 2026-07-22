#ifndef _SHAPE_H
#define _SHAPE_H

#include <stdint.h>

class Shape {
public:
  int16_t xPos = 0;
  int16_t yPos = 0;
  int16_t prevX, prevY;
  int16_t speedX = 1;
  int16_t speedY = 1;
  int16_t baseSpeedX = 1;
  int16_t baseSpeedY = 1;
  int sizeShape = 6;
  int16_t nextX = 0;
  int16_t nextY = 0;
  bool isColliding = false;

  Shape() = default;
  Shape(int16_t x, int16_t y, int16_t sx, int16_t sy, int sShape)
    : xPos(x), yPos(y), speedX(sx), speedY(sy),
      baseSpeedX(sx), baseSpeedY(sy), sizeShape(sShape) {}
};

#endif
