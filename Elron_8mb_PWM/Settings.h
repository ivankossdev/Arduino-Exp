#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

// --- Аппаратные настройки ---
const int ledPin = 9;                 // ШИМ-вывод D9

// --- Настройки «дыхания» ---
const uint8_t kBreathSpeed = 50;      // 0..100: больше — быстрее
const int     kFadeAmount  = 5;       // Шаг яркости за один тик (1..255)

// --- Границы ШИМ ---
const int kBrightnessMin = 0;
const int kBrightnessMax = 255;

// --- Гамма-коррекция ---
const float kGamma = 2.2f;            // 1.0 — линейно, 2.2 — естественно для глаза

#endif // SETTINGS_H