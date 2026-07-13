#include "mv.h"

void Logic_1();

void setup() {
  mshInit();
}

void loop() {
  Logic_1();
}

void Logic_1() {
  static size_t currentPointIndex = 0;

  // Если движения нет и мы не на паузе — ставим новую цель
  if (targetX == -1 && targetY == -1 && !isStopped) {
    targetX = route[currentPointIndex].x;
    targetY = route[currentPointIndex].y;
    currentPointIndex = (currentPointIndex + 1) % ROUTE_LENGTH;
  }

  // Двигаемся к цели
  stepTowardsTarget();

  // Если доехали (stepTowardsTarget сбросил target в -1) и пауза ещё не началась
  if (targetX == -1 && targetY == -1 && !isStopped) {
    isStopped = true;
    lastStopTime = millis();
  }

  // Проверяем, прошла ли пауза
  if (isStopped && (millis() - lastStopTime >= STOP_DURATION)) {
    isStopped = false;
    // Следующая точка поставится на следующем проходе, когда targetX/Y снова -1
  }
}



