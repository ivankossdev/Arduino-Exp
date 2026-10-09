#include "BlinkLed.h"
#include "Settings.h"
#include "EventTimer.h"

// ============================================================
//  Приватное состояние модуля (видно только внутри этого файла)
// ============================================================
namespace {

bool          gBlinkState = false;      // Текущее состояние пина
unsigned long gInterval   = kBlinkInterval;
EventTimer    gTicker;

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

  gInterval = kBlinkInterval;
  gTicker.setInterval(gInterval);
  gTicker.setCallback(onBlinkTick);
  gTicker.start();
}

void blinkUpdate() {
  gTicker.tick();
}

// ------------------------------------------------------------
//  Управление на лету
// ------------------------------------------------------------

void blinkSetInterval(unsigned long intervalMs) {
  gInterval = (intervalMs == 0) ? 1 : intervalMs;
  gTicker.setInterval(gInterval);   // сетка перезапустится
}

unsigned long blinkGetInterval() {
  return gInterval;
}

void blinkStart() { gTicker.start(); }
void blinkStop()  { gTicker.stop();  }
bool blinkIsRunning() { return gTicker.isRunning(); }