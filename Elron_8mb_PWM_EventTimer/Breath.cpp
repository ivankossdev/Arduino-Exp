#include "Breath.h"
#include "Settings.h"
#include "EventTimer.h"

// ============================================================
//  Приватное состояние модуля (видно только внутри этого файла)
// ============================================================
namespace {

int     gBrightness = 0;                 // Текущая яркость 0..255
int     gDirection  = 1;                 // +1 — разгорается, -1 — гаснет
uint8_t gGammaLut[256];                  // Таблица гамма-коррекции

uint8_t gSpeed = kBreathSpeed;           // Текущая скорость (0..100)
EventTimer gTicker;

// Пересчитать интервал тикера из текущей скорости.
void applySpeed() {
  // 0 -> медленно (50 мс),  100 -> быстро (3 мс)
  gTicker.setInterval(map(gSpeed, 0, 100, 50, 3));
}

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

  gSpeed = kBreathSpeed;
  applySpeed();
  gTicker.setCallback(onBreathTick);
  gTicker.start();
}

void breathUpdate() {
  gTicker.tick();
}

// ------------------------------------------------------------
//  Управление на лету
// ------------------------------------------------------------

void breathSetSpeed(uint8_t speed) {
  if (speed > 100) speed = 100;
  gSpeed = speed;
  applySpeed();   // setInterval() сам перезапустит сетку
}

uint8_t breathGetSpeed() {
  return gSpeed;
}

void breathStart() { gTicker.start(); }
void breathStop()  { gTicker.stop();  }
bool breathIsRunning() { return gTicker.isRunning(); }