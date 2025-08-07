#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayInit(Adafruit_SSD1306 *disp);
void progressBar(Adafruit_SSD1306 *disp, int item);
void clearProgressBar(Adafruit_SSD1306 *disp);
void showTotal(Adafruit_SSD1306 *disp, int item);
void clearTotal(Adafruit_SSD1306 *disp);
void moveShape(Adafruit_SSD1306 *disp, int mvX);

#endif