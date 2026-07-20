#ifndef _WALL_H
#define _WALL_H

#include "Arduino.h"

class Wall {
public:
  int16_t x;
  int16_t y;
  int16_t w; // ширина
  int16_t h; // высота

  // Конструктор
  Wall(int16_t _x, int16_t _y, int16_t _w, int16_t _h)
    : x(_x), y(_y), w(_w), h(_h) {}

  // Конструктор по умолчанию (нужен, если используешь массив)
  Wall() = default;
};

#endif
