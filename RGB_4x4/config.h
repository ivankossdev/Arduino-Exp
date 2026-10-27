#pragma once

// ---------- RGB ----------
#define LED_PIN 6
#define LED_COUNT 16

// ---------- Кнопки / джойстик ----------
#define BTN_ACTIVE_LOW 1

#define BTN_UP    2
#define BTN_DOWN  3
#define BTN_LEFT  4
#define BTN_RIGHT 5
#define BTN_MODE  7

#define START_POS 6

// ---------- Цвета точки (RGB) ----------
#define COLOR_DOT_R   0
#define COLOR_DOT_G   0
#define COLOR_DOT_B   255

#define COLOR_FLASH_R 255
#define COLOR_FLASH_G 0
#define COLOR_FLASH_B 0

// ---------- Цвета игровых элементов ----------
#define COLOR_TARGET_R 0
#define COLOR_TARGET_G 255
#define COLOR_TARGET_B 0

// Вспышка при достижении цели (белая — заметнее всего)
#define COLOR_GOAL_R 255
#define COLOR_GOAL_G 255
#define COLOR_GOAL_B 255

// ---------- Длительности вспышек ----------
#define STANDSTILL_FLASH_MS 150
#define GOAL_FLASH_MS       300
