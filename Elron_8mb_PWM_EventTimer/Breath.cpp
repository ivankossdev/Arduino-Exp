#include "Breath.h"
#include "Settings.h"
#include "EventTimer.h"

// ============================================================
//  Приватное состояние модуля (видно только внутри этого файла)
// ============================================================
namespace {

int     gBrightness = 0;          // Текущая яркость 0..255
int     gDirection  = 1;          // +1 — разгорается, -1 — гаснет
uint8_t gGammaLut[256];           // Таблица гамма-коррекции

EventTimer gTicker;               // дефолтный конструктор

// Один шаг «дыхания». Таймер сообщает «пора», мы делаем шаг.
void onBreathTick() {
  analogWrite(ledPin, gGammaLut[gBrightness]);

  gBrightness += kFadeAmount * gDirection;

  if (gBrightness >= kBrightnessMax) {
    gBrightness = kBrightnessMax;
    gDirection  = -1;
  } else if (gBrightness <= kBrightnessMin) {
    gBrightness = kBrightnessMin;
    gDirection  = 1;
  }
}

}  // namespace

// ============================================================
//  Публичный интерфейс
// ============================================================

void breathBegin() {
  pinMode(ledPin, OUTPUT);

  // Таблица гамма-коррекции — один раз при старте.
  for (int i = 0; i < 256; i++) {
    float v = i / 255.0f;
    v = pow(v, kGamma);
    gGammaLut[i] = (uint8_t)(v * 255.0f + 0.5f);
  }

  // Скорость 0..100 -> миллисекунды на шаг (0 -> медленно, 100 -> быстро).
  gTicker.setInterval(map(kBreathSpeed, 0, 100, 50, 3));
  gTicker.setCallback(onBreathTick);
  gTicker.start();
}

void breathUpdate() {
  gTicker.tick();
}