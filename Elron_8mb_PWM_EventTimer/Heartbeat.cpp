#include "Heartbeat.h"
#include "EventTimer.h"
#include "Settings.h"

// ============================================================
//  Приватное состояние модуля (видно только внутри этого файла)
// ============================================================
namespace {

unsigned long gInterval = kHeartbeatInterval;
EventTimer    gTicker;

void onHeartbeat() {
  unsigned long ms  = millis();
  unsigned long sec = ms / 1000;
  unsigned long min = sec / 60;
  unsigned long hrs = min / 60;

  Serial.print(F("[HB] up="));
  Serial.print(hrs);
  Serial.print('h');
  Serial.print(min % 60);
  Serial.print('m');
  Serial.print(sec % 60);
  Serial.println('s');
}

}  // namespace

// ============================================================
//  Публичный интерфейс
// ============================================================

void heartbeatBegin() {
  gInterval = kHeartbeatInterval;
  gTicker.setInterval(gInterval);
  gTicker.setCallback(onHeartbeat);
  gTicker.start();
}

void heartbeatUpdate() {
  gTicker.tick();
}

// ------------------------------------------------------------
//  Управление на лету
// ------------------------------------------------------------

void heartbeatSetInterval(unsigned long intervalMs) {
  gInterval = (intervalMs == 0) ? 1 : intervalMs;
  gTicker.setInterval(gInterval);
}

unsigned long heartbeatGetInterval() {
  return gInterval;
}

void heartbeatStart() { gTicker.start(); }
void heartbeatStop()  { gTicker.stop();  }
bool heartbeatIsRunning() { return gTicker.isRunning(); }