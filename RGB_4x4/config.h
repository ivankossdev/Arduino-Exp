#pragma once

// ---------- RGB ----------
#define LED_PIN 6
#define LED_COUNT 16

// ---------- Кнопки / джойстик ----------
// 1 = общий провод GND, INPUT_PULLUP, нажатие = LOW  (стандарт)
// 0 = общий провод VCC, внешние pull-down, нажатие = HIGH
#define BTN_ACTIVE_LOW 1

#define BTN_UP    2
#define BTN_DOWN  3
#define BTN_LEFT  4
#define BTN_RIGHT 5
#define BTN_MODE  7

#define START_POS 6