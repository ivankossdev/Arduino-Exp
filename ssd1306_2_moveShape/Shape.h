#pragma once
#include <stdint.h>

class Shape {
public:
  int16_t xPos = 0;
  int16_t yPos = 0;

  int16_t speedX = 1;
  int16_t speedY = 1;

  // Вот это — «изначальная» скорость, которую хотим вернуть у стены
  int16_t baseSpeedX = 1;
  int16_t baseSpeedY = 1;

  int sizeShape = 6;

  Shape(int16_t x, int16_t y, int16_t sx, int16_t sy, int sShape) 
    : xPos(x), yPos(y), speedX(sx), speedY(sy), 
      baseSpeedX(sx), baseSpeedY(sy), sizeShape(sShape) {}

  Shape() = default;
};