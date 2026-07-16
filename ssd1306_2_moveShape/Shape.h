#pragma once
#include <stdint.h>

class Shape {
public:
  int16_t xPos = 0;
  int16_t yPos = 0;
  int16_t speedX = 1;
  int16_t speedY = 1;

  Shape(int16_t x, int16_t y, int16_t sx, int16_t sy) 
    : xPos(x), yPos(y), speedX(sx), speedY(sy) {}
  Shape() = default;
};