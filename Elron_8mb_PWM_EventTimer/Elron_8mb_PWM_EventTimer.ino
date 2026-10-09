#include <Arduino.h>

#include "Settings.h"
#include "Breath.h"
#include "BlinkLed.h"

/***********************************************************
  Elbear Ace-UNO:
    - плавное «дыхание» внешнего светодиода на D9 (ШИМ);
    - неблокирующая мигалка встроенного светодиода.

  Вся логика — в модулях Breath и BlinkLed.
  Настройки — в Settings.h.
  loop() не блокируется: обе задачи работают параллельно.
 ***********************************************************/

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Start program!");

  breathBegin();
  blinkBegin();
}

void loop() {
  breathUpdate();
  blinkUpdate();
}