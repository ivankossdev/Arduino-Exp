#include "mv.h"

void Logic_1() {
  mShape.xPos += mShape.speedX;
  mShape.yPos += mShape.speedY;

  // Отскок по X + выталкивание на границу
  if (mShape.xPos <= MIN_XPOS) {
    mShape.xPos = MIN_XPOS;
    mShape.speedX = -mShape.speedX;
  } else if (mShape.xPos + SIZE_SHAPE >= SCREEN_WIDTH) {
    mShape.xPos = MAX_XPOS;          // SCREEN_WIDTH - SIZE_SHAPE
    mShape.speedX = -mShape.speedX;
  }

  // Отскок по Y + выталкивание на границу (с учётом отступа под текст)
  if (mShape.yPos <= MIN_YPOS) {
    mShape.yPos = MIN_YPOS;
    mShape.speedY = -mShape.speedY;
  } else if (mShape.yPos + SIZE_SHAPE >= SCREEN_HEIGHT) {
    mShape.yPos = MAX_YPOS;          // SCREEN_HEIGHT - SIZE_SHAPE
    mShape.speedY = -mShape.speedY;
  }
}



void setup() {
  mshInit();
}

void loop() {
  Logic_1(); // только двигаем координаты и считаем паузы
  mShape.drawFrame(); 
}
