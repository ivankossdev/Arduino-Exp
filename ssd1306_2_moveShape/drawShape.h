#ifndef _DRAWSHAPE_H
#define _DRAWSHAPE_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"
#include "config.h"
#include "Shape.h"

class Shape;

class DrawShape {
public:
  Adafruit_SSD1306 *oled;
  int width, height;

  DrawShape(int w, int h, TwoWire *wire, int rst);

  void clearScreen();
  void show();
  void drawFrame(const Shape& s);
  void dispCord(const Shape& s);

  // Метод для рисования прямоугольника
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    oled->drawRect(x, y, w, h, color);
  }
};

#endif
