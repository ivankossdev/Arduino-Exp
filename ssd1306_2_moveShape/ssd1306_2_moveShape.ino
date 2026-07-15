#include "mv.h"

MShape mShape1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MShape mShape2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void Logic_1() {
  updateShape(mShape1);
  updateShape(mShape2);
  mShape1.drawFrame();
  mShape2.drawFrame();
  checkCollisionBetween(mShape1, mShape2);
}

// Вспомогательная функция: двигаем, проверяем стены, выталкиваем
void updateShape(MShape& s) {
  s.xPos += s.speedX;
  s.yPos += s.speedY;

  // Отскок от левой/правой
  if (s.xPos <= MIN_XPOS) {
    s.xPos = MIN_XPOS;
    s.speedX = -s.speedX;
  } else if (s.xPos + SIZE_SHAPE >= SCREEN_WIDTH) {
    s.xPos = MAX_XPOS;
    s.speedX = -s.speedX;
  }

  // Отскок от верхней/нижней (с учётом отступа под текст)
  if (s.yPos <= MIN_YPOS) {
    s.yPos = MIN_YPOS;
    s.speedY = -s.speedY;
  } else if (s.yPos + SIZE_SHAPE >= SCREEN_HEIGHT) {
    s.yPos = MAX_YPOS;
    s.speedY = -s.speedY;
  }
}

void checkCollisionBetween(MShape& a, MShape& b) {
  // Простая AABB-проверка: прямоугольники пересекаются?
  bool xOverlap = a.xPos < b.xPos + SIZE_SHAPE &&
                  b.xPos < a.xPos + SIZE_SHAPE;

  bool yOverlap = a.yPos < b.yPos + SIZE_SHAPE &&
                  b.yPos < a.yPos + SIZE_SHAPE;

  if (xOverlap && yOverlap) {
    // Меняем скорости на противоположные у обоих
    a.speedX = -a.speedX;
    a.speedY = -a.speedY;
    b.speedX = -b.speedX;
    b.speedY = -b.speedY;

    // Выталкиваем друг от друга, чтобы не «залипли» в одном кадре
    // Вариант 1: просто раздвигаем по осям (простой, но может выглядеть чуть неестественно)
    if (a.xPos < b.xPos) {
      a.xPos -= 1;
      b.xPos += 1;
    } else {
      a.xPos += 1;
      b.xPos -= 1;
    }

    if (a.yPos < b.yPos) {
      a.yPos -= 1;
      b.yPos += 1;
    } else {
      a.yPos += 1;
      b.yPos -= 1;
    }
  }
}

void setup() {
  mShape1.init();
  mShape2.init();

  // Разнесём их по разным углам, чтобы сразу не столкнулись
  mShape1.xPos = 10;
  mShape1.yPos = 20;
  mShape1.speedX = 2;
  mShape1.speedY = 3;

  mShape2.xPos = 100;
  mShape2.yPos = 40;
  mShape2.speedX = -3;
  mShape2.speedY = -2;
}

void loop() {
  Logic_1();
  
}

