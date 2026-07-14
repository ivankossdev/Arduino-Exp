#include "mv.h"

void Logic_1() {
  static size_t currentPointIndex = 0;

  if (targetX == -1 && targetY == -1 && !isStopped) {
    targetX = route[currentPointIndex].x;
    targetY = route[currentPointIndex].y;
    currentPointIndex = (currentPointIndex + 1) % ROUTE_LENGTH;
  }

  stepTowardsTarget();

  if (targetX == -1 && targetY == -1 && !isStopped) {
    isStopped = true;
    lastStopTime = millis();
  }

  if (isStopped && (millis() - lastStopTime >= STOP_DURATION)) {
    isStopped = false;
  }
}

void setup() {
  mshInit();
}

void loop() {
  Logic_1(); // только двигаем координаты и считаем паузы
  
  mShape.drawFrame(isStopped); 
}
