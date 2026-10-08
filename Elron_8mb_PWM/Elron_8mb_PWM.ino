#include "Settings.h"

/*
  Плавное мигание светодиода (PWM) на Elbear Ace-UNO без блокировки loop()
  Подключение: анод светодиода через резистор 220-330 Ом к D9, катод к GND.
*/

// ============================================================
//  Модуль «дыхания светодиода»
// ============================================================

// --- Состояние (скрыто внутри модуля) ---
static int           gBrightness     = 0;   // Текущая яркость 0..255
static int           gDirection      = 1;   // +1 — разгорается, -1 — гаснет
static unsigned long gPreviousMillis = 0;   // Момент последнего шага
static unsigned long gInterval       = 30;  // мс между шагами (из kBreathSpeed)
static uint8_t       gGammaLut[256];        // Таблица гамма-коррекции

// --- Инициализация: вызывать один раз в setup() ---
static void breathBegin() {
  pinMode(ledPin, OUTPUT);

  // Строим таблицу гамма-коррекции один раз при старте
  for (int i = 0; i < 256; i++) {
    float v = i / 255.0f;
    v = pow(v, kGamma);
    gGammaLut[i] = (uint8_t)(v * 255.0f + 0.5f);
  }

  // Переводим «человеческую» скорость в миллисекунды на шаг
  // 0  -> медленно (50 мс),  100 -> быстро (3 мс)
  gInterval = map(kBreathSpeed, 0, 100, 50, 3);
}

// --- Обновление: вызывать в loop() как можно чаще ---
static void breathUpdate() {
  unsigned long now = millis();
  if (now - gPreviousMillis < gInterval) return;  // ещё не время
  gPreviousMillis = now;

  analogWrite(ledPin, gGammaLut[gBrightness]);    // вывести с гамма-коррекцией

  gBrightness += kFadeAmount * gDirection;

  if (gBrightness >= kBrightnessMax) {
    gBrightness = kBrightnessMax;
    gDirection  = -1;
  } else if (gBrightness <= kBrightnessMin) {
    gBrightness = kBrightnessMin;
    gDirection  = 1;
  }
}

// ============================================================
//  Точка входа Arduino
// ============================================================

void setup() {
  breathBegin();
}

void loop() {
  breathUpdate();
  // Здесь можно добавлять любые другие задачи — loop() не блокируется.
}