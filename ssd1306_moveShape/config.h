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

// Новая цель движения (если targetX == -1 — движения нет)
extern int16_t targetX;
extern int16_t targetY;
constexpr int16_t STEP_SIZE = 2; // сколько пикселей за один loop()

// Целевые позиции для паттерна (чтобы не держать в голове 117/54 и т.п.)
constexpr int16_t TARGET_TOP_LEFT_X  = MIN_XPOS;
constexpr int16_t TARGET_TOP_RIGHT_X = MAX_XPOS;
constexpr int16_t TARGET_BOTTOM_LEFT_X  = MIN_XPOS;
constexpr int16_t TARGET_BOTTOM_RIGHT_X = MAX_XPOS;

constexpr int16_t TARGET_TOP_Y    = MIN_YPOS;
constexpr int16_t TARGET_BOTTOM_Y = MAX_YPOS;

struct Point {
  int16_t x;
  int16_t y;
};

// Маршрут: квадрат будет ехать по этим точкам по порядку
const Point route[] = {
  { TARGET_TOP_LEFT_X,   TARGET_TOP_Y },      // 0: левый верх
  { TARGET_TOP_RIGHT_X,  TARGET_TOP_Y },      // 1: правый верх
  { TARGET_BOTTOM_RIGHT_X, TARGET_BOTTOM_Y }, // 2: правый низ
  { TARGET_BOTTOM_LEFT_X,  TARGET_BOTTOM_Y }  // 3: левый низ
};

constexpr size_t ROUTE_LENGTH = sizeof(route) / sizeof(route[0]);

extern unsigned long lastStopTime;
extern bool isStopped;
constexpr unsigned long STOP_DURATION = 50;  

// Переменная для пульсации
extern bool isPulseGrowing;

#endif