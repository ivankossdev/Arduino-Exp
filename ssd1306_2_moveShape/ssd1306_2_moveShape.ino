#include "mv.h"

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 20, 2, 3), 
  Shape(100, 40, 1, 1),
  Shape(60, 30, 2, 1),
  Shape(20, 20, 2, 2),
  Shape(10, 40, -2, -3),
  Shape(0, 40, -3, -3)
}; 

int shapeCount = 6; 

void Logic_1() {
  // 1. Двигаем каждую фигуру
  for (int i = 0; i < shapeCount; ++i) {
    updateShape(shapes[i]);
  }

  // 2. Проверяем столкновения между всеми парами
  for (int i = 0; i < shapeCount; ++i) {
    for (int j = i + 1; j < shapeCount; ++j) {
      checkCollisionBetween(shapes[i], shapes[j]);
    }
  }
}


void drawFrame() {
  drawShape.clearScreen();

  for (int i = 0; i < shapeCount; ++i) {
    // Можно сделать цвета по кругу, чтобы было видно все фигуры
    drawShape.drawFrame(shapes[i]);
  }

  drawShape.dispCord(shapes[0]); // координаты первой фигуры (или любой другой)
  drawShape.show();
}


// Вспомогательная функция: двигаем, проверяем стены, выталкиваем
void updateShape(Shape& s) {
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


void checkCollisionBetween(Shape& a, Shape& b) {
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
  drawShape.init();
}

void loop() {
  Logic_1();
  drawFrame();
}

