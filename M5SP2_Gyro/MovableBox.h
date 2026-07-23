#pragma once
#include <M5Unified.h>
#include "Wall.h"

class Wall;

class MovableBox {
public:
  int16_t x, y;
  int16_t w, h;
  float vx, vy;          // скорость теперь float для точности
  int16_t prevX, prevY;  // старые координаты для очистки

  MovableBox(int16_t _x, int16_t _y, int16_t _w, int16_t _h);

  void update();
  void bounceEdges(int16_t screenW, int16_t screenH, const Wall* walls, int wallCount, float bounceFactor);

  void clear(M5GFX &lcd, uint16_t bgColor);
  void draw(M5GFX &lcd, uint16_t outlineColor, uint16_t fillColor);
};
