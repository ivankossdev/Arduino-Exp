#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>   // для int16_t, uint8_t

// Размеры дисплея
constexpr int SCREEN_WIDTH  = 128;
constexpr int SCREEN_HEIGHT = 64;

// Параметры подключения OLED
constexpr int OLED_RESET      = -1;
constexpr uint8_t SCREEN_ADDRESS = 0x3C;

constexpr int SIZE_SHAPE = 7;

constexpr int16_t MAX_XPOS = SCREEN_WIDTH - SIZE_SHAPE;   
constexpr int16_t MIN_XPOS = 0;
constexpr int16_t MAX_YPOS = SCREEN_HEIGHT - SIZE_SHAPE;
constexpr int16_t MIN_YPOS = 16;    // отступ сверху для строки координат

// Новая цель движения (если targetX == -1 — движения нет)
extern int16_t targetX;
extern int16_t targetY;
constexpr int16_t STEP_SIZE = 2; // сколько пикселей за один loop()

#endif