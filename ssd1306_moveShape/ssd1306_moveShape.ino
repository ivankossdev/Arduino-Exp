#include "mv.h"

void Logic_1();
void Logic_2();

void setup() {
  mshInit();
}

void loop() {
  Logic_2();
}

void Logic_1() {
  downAndRight();
  upAndRight();

  downAndRight();
  mvUp();

  downAndLeft();
  upAndLeft();

  downAndLeft();
  mvUp();
}

void Logic_2() {
  // Если движения нет — ставим новую цель
  if (targetX == -1 && targetY == -1) {
    static uint8_t patternIndex = 0;

    switch (patternIndex) {
      case 0:
        // В правый верхний угол (почти downAndRight)
        targetX = TARGET_TOP_RIGHT_X;
        targetY = TARGET_TOP_Y;
        break;

      case 1:
        // В правый нижний угол (upAndRight по старой терминологии)
        targetX = TARGET_BOTTOM_RIGHT_X;
        targetY = TARGET_BOTTOM_Y;
        break;

      case 2:
        // В левый нижний угол
        targetX = TARGET_BOTTOM_LEFT_X;
        targetY = TARGET_BOTTOM_Y;
        break;

      case 3:
        // В левый верхний угол
        targetX = TARGET_TOP_LEFT_X;
        targetY = TARGET_TOP_Y;
        break;

      default:
        patternIndex = 0;
        // Зацикливаем паттерн
        return;
    }
    patternIndex++;
  }

  // Каждый loop() делаем один шаг к текущей цели
  stepTowardsTarget();
}
