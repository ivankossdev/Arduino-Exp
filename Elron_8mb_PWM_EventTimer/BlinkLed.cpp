#include "BlinkLed.h"
#include "Settings.h"
#include "EventTimer.h"

// ============================================================
//  Приватное состояние модуля (видно только внутри этого файла)
// ============================================================
namespace {

// Тикер мигалки. Реальный интервал выставим в blinkBegin().
EventTimer gTicker(1);

// Один тик мигалки. Таймер уже инвертировал своё состояние —
// используем его как целевое состояние пина.
void onBlinkTick(bool state) {
  digitalWrite(kBlinkPin, state ? HIGH : LOW);
}

}  // namespace

// ============================================================
//  Публичный интерфейс
// ============================================================

void blinkBegin() {
  pinMode(kBlinkPin, OUTPUT);
  digitalWrite(kBlinkPin, LOW);   // стартуем с погашенного светодиода

  gTicker.setInterval(kBlinkInterval);
  gTicker.setCallback(onBlinkTick);
  gTicker.start();
}

void blinkUpdate() {
  gTicker.tick();
}