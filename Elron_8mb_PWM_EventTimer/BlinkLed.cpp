#include "BlinkLed.h"
#include "Settings.h"
#include "EventTimer.h"

// ============================================================
//  Приватное состояние модуля (видно только внутри этого файла)
// ============================================================
namespace {

bool      gBlinkState = false;   // текущее состояние пина
EventTimer gTicker;              // дефолтный конструктор — наконец-то!

// Один тик мигалки: инвертируем состояние и выводим на пин.
void onBlinkTick() {
  gBlinkState = !gBlinkState;
  digitalWrite(kBlinkPin, gBlinkState ? HIGH : LOW);
}

}  // namespace

// ============================================================
//  Публичный интерфейс
// ============================================================

void blinkBegin() {
  pinMode(kBlinkPin, OUTPUT);
  gBlinkState = false;
  digitalWrite(kBlinkPin, LOW);   // стартуем с погашенного светодиода

  gTicker.setInterval(kBlinkInterval);
  gTicker.setCallback(onBlinkTick);
  gTicker.start();
}

void blinkUpdate() {
  gTicker.tick();
}