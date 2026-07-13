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

  // Если нет активной цели — ставим следующую из маршрута
  if (targetX == -1 && targetY == -1) {
    // Берем точку из массива
    targetX = route[currentPointIndex].x;
    targetY = route[currentPointIndex].y;

    // Готовим индекс для следующей цели (закольцовываем маршрут)
    currentPointIndex = (currentPointIndex + 1) % ROUTE_LENGTH;
  }

  stepTowardsTarget();
}


