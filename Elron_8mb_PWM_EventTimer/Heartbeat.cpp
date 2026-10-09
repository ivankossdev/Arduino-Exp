#include "Heartbeat.h"
#include "EventTimer.h"
#include "Settings.h"


namespace {
  EventTimer hBTicker;

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
} // namespace

void heartbeatBegin(){
  hBTicker.setInterval(kHeartbeatInterval); 
  hBTicker.setCallback(onHeartbeat);
  hBTicker.start();
}

void heartbeatUpdate(){
  hBTicker.tick();
}