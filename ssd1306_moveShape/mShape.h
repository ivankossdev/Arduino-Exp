#ifndef _MSHAPE_H
#define _MSHAPE_H
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"
#include "config.h"


enum Display {
  on, off
}; 

class MShape : private Adafruit_SSD1306 {
public:
  using Adafruit_SSD1306::Adafruit_SSD1306;
  void init();
  void dispCord();
  void movement(int16_t x, int16_t y);
  void clearSh(Display dsp);
  void setDefaultXPos();
  void setDefaultYPos();
  void clear();
  int16_t xPos;
  int16_t yPos;
  void updateDisplay() { display(); }
private:
  int16_t xOldPos;
  int16_t yOldPos;
};

#endif