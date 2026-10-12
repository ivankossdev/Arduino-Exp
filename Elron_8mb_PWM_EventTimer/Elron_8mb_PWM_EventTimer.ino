#include <Arduino.h>

#include "Settings.h"
#include "Breath.h"
#include "BlinkLed.h"
#include "Heartbeat.h"
#include "SerialConsole.h"

/***********************************************************
  Elbear Ace-UNO:
    - плавное «дыхание» внешнего светодиода на D9 (ШИМ);
    - неблокирующая мигалка встроенного светодиода;
    - Heartbeat: печатает аптайм в Serial;
    - SerialConsole: управление параметрами с клавиатуры.

  Вся логика — в модулях Breath / BlinkLed / Heartbeat,
  дирижёр команд — SerialConsole, настройки — Settings.h.
  loop() не блокируется: все задачи работают параллельно.
 ***********************************************************/

void setup() {
  Serial.begin(9600);

  breathBegin();
  blinkBegin();
  heartbeatBegin();
  consoleBegin();
}

void loop() {
  breathUpdate();
  blinkUpdate();
  heartbeatUpdate();
  consoleUpdate();
}