#pragma once
#include <Arduino.h>

enum ButtonEvent {
  BTN_NONE = 0,
  BTN1_PRESSED,
  BTN2_PRESSED
};

void buttonsInit();
ButtonEvent buttonsPoll();