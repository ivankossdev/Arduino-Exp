#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>

// Настройки дисплея 
constexpr int SCREEN_WIDTH  = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int OLED_RESET      = -1;
constexpr uint8_t SCREEN_ADDRESS = 0x3C;

// Настройки координат
constexpr int16_t MIN_XPOS = 0;
// отступ сверху для строки координат
constexpr int16_t MIN_YPOS = 16;    

constexpr int MAX_SHAPES = 10;

#endif