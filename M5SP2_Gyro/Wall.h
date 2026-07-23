#pragma once
#include <M5Unified.h>

class Wall {
public:
  int16_t x, y, w, h;

  Wall(int16_t _x, int16_t _y, int16_t _w, int16_t _h)
    : x(_x), y(_y), w(_w), h(_h) {}

  void draw(M5GFX &lcd, uint16_t color) const;
};
