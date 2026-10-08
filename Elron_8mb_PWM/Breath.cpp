#include "Breath.h"
#include "Settings.h"

// ============================================================
//  Приватное состояние модуля (видно только внутри этого файла)
// ============================================================
namespace {

int           gBrightness     = 0;   // Текущая яркость 0..255
int           gDirection      = 1;   // +1 — разгорается, -1 — гаснет
unsigned long gPreviousMillis = 0;   // Момент последнего шага
unsigned long gInterval       = 30;  // мс между шагами (из kBreathSpeed)
uint8_t       gGammaLut[256];        // Таблица гамма-коррекции

}  // namespace

// ============================================================
//  Публичный интерфейс
// ============================================================

void breathBegin() {
  pinMode(ledPin, OUTPUT);

  // Таблицу гамма-коррекции строим один раз при старте.
  for (int i = 0; i < 256; i++) {
    float v = i / 255.0f;
    v = pow(v, kGamma);
    gGammaLut[i] = (uint8_t)(v * 255.0f + 0.5f);
  }

  // «Человеческую» скорость 0..100 переводим в миллисекунды на шаг.
  // 0 -> медленно (50 мс),  100 -> быстро (3 мс)
  gInterval = map(kBreathSpeed, 0, 100, 50, 3);
}

void breathUpdate() {
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