#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>   // для int16_t, uint8_t

// Размеры дисплея
constexpr int SCREEN_WIDTH  = 128;
constexpr int SCREEN_HEIGHT = 64;

// Параметры подключения OLED
constexpr int OLED_RESET      = -1;
constexpr uint8_t SCREEN_ADDRESS = 0x3C;
  
constexpr int16_t MIN_XPOS = 0;
constexpr int16_t MIN_YPOS = 16;    // отступ сверху для строки координат
constexpr int16_t STEP_SIZE = 1; // сколько пикселей за один loop()

constexpr int MAX_SHAPES = 10;
// constexpr int16_t SHAPEMASS = 1;  // <-- закомментируй это
constexpr int16_t SHAPEMASS_LIGHT = 1;
constexpr int16_t SHAPEMASS_MED = 3;
constexpr int16_t SHAPEMASS_HEAVY = 8;


#endif