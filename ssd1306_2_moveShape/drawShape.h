#ifndef _DRAWSHAPE_H
#define _DRAWSHAPE_H
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"
#include "config.h"
#include "Shape.h"


class DrawShape : private Adafruit_SSD1306 {
public:
  using Adafruit_SSD1306::Adafruit_SSD1306;
  void init();
  void dispCord(Shape& shape);
  void movement(Shape& shape, int16_t x, int16_t y);
  void clearSh(Shape& shape);
  void show() {
    display();
  }
  void drawFrame(Shape& shape);
};

#endif