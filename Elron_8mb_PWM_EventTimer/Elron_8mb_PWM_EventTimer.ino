#include <Arduino.h>

#include "Settings.h"
#include "Breath.h"
#include "BlinkLed.h"
#include "Heartbeat.h"

/***********************************************************
  Elbear Ace-UNO:
    - плавное «дыхание» внешнего светодиода на D9 (ШИМ);
    - неблокирующая мигалка встроенного светодиода;
    - Heartbeat: раз в 2 с печатает аптайм в Serial.

  Вся логика — в модулях Breath, BlinkLed, Heartbeat.
  Настройки — в Settings.h.
  loop() не блокируется: три задачи работают параллельно.
 ***********************************************************/

void setup() {
  Serial.begin(9600);
  breathBegin();
  blinkBegin();
  heartbeatBegin();
}

void loop() {
  breathUpdate();
  blinkUpdate();
  heartbeatUpdate();
}