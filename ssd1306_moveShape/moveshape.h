#ifndef _MOVESHAPE_H
#define _MOVESHAPE_H
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

enum Display {
  on, off
}; 

class MoveShape : private Adafruit_SSD1306 {
public:
  using Adafruit_SSD1306::Adafruit_SSD1306;
  void init();
  void dispCord();
  void movement(int16_t x, int16_t y);
  void clearSh(Display dsp);
  void serDefaultXPos();
  void serDefaultYPos();
  void TestEnum(Display dsp);
  int16_t xPos;
  int16_t yPos;
private:
  int16_t xOldPos;
  int16_t yOldPos;
};

#endif