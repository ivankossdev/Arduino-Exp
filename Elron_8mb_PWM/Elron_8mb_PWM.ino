#include "Breath.h"

/*
  Плавное мигание светодиода (PWM) на Elbear Ace-UNO без блокировки loop().
  Подключение: анод светодиода через резистор 220-330 Ом к D9, катод к GND.

  Вся логика «дыхания» вынесена в модуль Breath (Breath.h / Breath.cpp).
  Настройки — в Settings.h.
*/

void setup() {
  breathBegin();
}

void loop() {
  breathUpdate();
  // Любые другие задачи можно дописывать здесь — loop() не блокируется.
}