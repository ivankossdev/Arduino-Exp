#ifndef _DRAWSHAPE_H
#define _DRAWSHAPE_H

#include <M5Unified.h>
#include "config.h"
#include "Shape.h"

class Shape;

class DrawShape {
public:
  int width, height;

  DrawShape(int w, int h);
  void clearScreen();
  void show();
  void drawFrame(const Shape& s);
  void dispCord(const Shape& s);
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
};

#endif
