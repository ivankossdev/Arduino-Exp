#ifndef _WALL_H
#define _WALL_H

#include <Arduino.h>

class Wall {
public:
  int16_t x, y, w, h;

  Wall() = default;
  Wall(int16_t _x, int16_t _y, int16_t _w, int16_t _h)
    : x(_x), y(_y), w(_w), h(_h) {}
};

#endif
