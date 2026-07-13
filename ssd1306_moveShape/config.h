#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>   // для int16_t, uint8_t

// Размеры дисплея
constexpr int SCREEN_WIDTH  = 128;
constexpr int SCREEN_HEIGHT = 64;

// Параметры подключения OLED
constexpr int OLED_RESET      = -1;
constexpr uint8_t SCREEN_ADDRESS = 0x3C;

// Границы движения квадрата (размер квадрата 10x10)
constexpr int16_t MAX_XPOS = 117;   // ставим точно те же значения, что работали ранее
constexpr int16_t MIN_XPOS = 0;
constexpr int16_t MAX_YPOS = 54;
constexpr int16_t MIN_YPOS = 16;    // отступ сверху для строки координат

#endif