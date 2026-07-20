#include "mv.h"

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 20, 2, 3, 6),
  Shape(80, 40, 2, 3, 6),
  Shape(100, 40, 2, 1, 10),
  Shape(60, 30, -1, -1, 10),
};

int shapeCount = 4;
const int COLLISION_ITERATIONS = 10;

void Logic_1() {
  // 1. Предвычисление и коррекция будущих позиций
  predictAndResolveCollisions();

  // 2. Применяем исправленные позиции
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].xPos = shapes[i].nextX;
    shapes[i].yPos = shapes[i].nextY;
  }

  // 3. Ограничение стенами (без случайных скоростей)
  for (int i = 0; i < shapeCount; ++i) {
    constrainToWalls(shapes[i]);
  }
}


void constrainToWalls(Shape& s) {
  // Левая стена
  if (s.xPos <= MIN_XPOS) {
    s.xPos = MIN_XPOS;
    if (s.speedX < 0) {
      s.speedX = -s.speedX;  // отскок, только если летели в стену
    }
  }
  // Правая стена
  if (s.xPos + s.sizeShape >= SCREEN_WIDTH) {
    s.xPos = SCREEN_WIDTH - s.sizeShape;
    if (s.speedX > 0) {
      s.speedX = -s.speedX;
    }
  }
  // Верхняя стена
  if (s.yPos <= MIN_YPOS) {
    s.yPos = MIN_YPOS;
    if (s.speedY < 0) {
      s.speedY = -s.speedY;
    }
  }
  // Нижняя стена
  if (s.yPos + s.sizeShape >= SCREEN_HEIGHT) {
    s.yPos = SCREEN_HEIGHT - s.sizeShape;
    if (s.speedY > 0) {
      s.speedY = -s.speedY;
    }
  }
}


void resolveCollision(Shape& a, Shape& b) {
  bool xOverlap = a.xPos < b.xPos + b.sizeShape && b.xPos < a.xPos + a.sizeShape;
  bool yOverlap = a.yPos < b.yPos + b.sizeShape && b.yPos < a.yPos + a.sizeShape;
  if (!xOverlap || !yOverlap) return;

  int overlapX = 0;
  if (a.xPos < b.xPos) {
    overlapX = (a.xPos + a.sizeShape) - b.xPos;
  } else {
    overlapX = (b.xPos + b.sizeShape) - a.xPos;
  }

  int overlapY = 0;
  if (a.yPos < b.yPos) {
    overlapY = (a.yPos + a.sizeShape) - b.yPos;
  } else {
    overlapY = (b.yPos + b.sizeShape) - a.yPos;
  }

  // Выталкиваем по оси с МЕНЬШИМ перекрытием — это даёт более естественное поведение
  if (overlapX < overlapY) {
    int shift = overlapX / 2;
    if (a.xPos < b.xPos) {
      a.xPos -= shift;
      b.xPos += shift;
    } else {
      a.xPos += shift;
      b.xPos -= shift;
    }
    // Инвертируем только X-скорость
    a.speedX = -a.speedX;
    b.speedX = -b.speedX;
  } else {
    int shift = overlapY / 2;
    if (a.yPos < b.yPos) {
      a.yPos -= shift;
      b.yPos += shift;
    } else {
      a.yPos += shift;
      b.yPos -= shift;
    }
    a.speedY = -a.speedY;
    b.speedY = -b.speedY;
  }
}

void predictAndResolveCollisions() {
  // Сначала считаем, где фигуры будут на следующем кадре
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].nextX = shapes[i].xPos + shapes[i].speedX;
    shapes[i].nextY = shapes[i].yPos + shapes[i].speedY;
  }

  // Проверяем столкновения в будущих позициях
  for (int iter = 0; iter < COLLISION_ITERATIONS; ++iter) {
    bool anyCollision = false;

    for (int i = 0; i < shapeCount; ++i) {
      for (int j = i + 1; j < shapeCount; ++j) {
        // Проверка пересечения по будущим позициям
        bool xOverlap = shapes[i].nextX < shapes[j].nextX + shapes[j].sizeShape
                     && shapes[j].nextX < shapes[i].nextX + shapes[i].sizeShape;
        bool yOverlap = shapes[i].nextY < shapes[j].nextY + shapes[j].sizeShape
                     && shapes[j].nextY < shapes[i].nextY + shapes[i].sizeShape;

        if (!xOverlap || !yOverlap) continue;

        anyCollision = true;

        // Выбираем ось с меньшим перекрытием для коррекции
        int overlapX = 0;
        if (shapes[i].nextX < shapes[j].nextX) {
          overlapX = (shapes[i].nextX + shapes[i].sizeShape) - shapes[j].nextX;
        } else {
          overlapX = (shapes[j].nextX + shapes[j].sizeShape) - shapes[i].nextX;
        }

        int overlapY = 0;
        if (shapes[i].nextY < shapes[j].nextY) {
          overlapY = (shapes[i].nextY + shapes[i].sizeShape) - shapes[j].nextY;
        } else {
          overlapY = (shapes[j].nextY + shapes[j].sizeShape) - shapes[i].nextY;
        }

        if (overlapX < overlapY) {
          // Коррекция по X: ставим фигуры ровно на касание, инвертируем скорости
          if (shapes[i].nextX < shapes[j].nextX) {
            // i слева, j справа
            int contactX = shapes[j].nextX - shapes[i].sizeShape;  // где i должен остановиться
            shapes[i].nextX = contactX;
            shapes[j].nextX = shapes[i].nextX + shapes[i].sizeShape;
          } else {
            // j слева, i справа
            int contactX = shapes[i].nextX - shapes[j].sizeShape;
            shapes[j].nextX = contactX;
            shapes[i].nextX = shapes[j].nextX + shapes[j].sizeShape;
          }
          shapes[i].speedX = -shapes[i].speedX;
          shapes[j].speedX = -shapes[j].speedX;
        } else {
          // Коррекция по Y
          if (shapes[i].nextY < shapes[j].nextY) {
            int contactY = shapes[j].nextY - shapes[i].sizeShape;
            shapes[i].nextY = contactY;
            shapes[j].nextY = shapes[i].nextY + shapes[i].sizeShape;
          } else {
            int contactY = shapes[i].nextY - shapes[j].sizeShape;
            shapes[j].nextY = contactY;
            shapes[i].nextY = shapes[j].nextY + shapes[j].sizeShape;
          }
          shapes[i].speedY = -shapes[i].speedY;
          shapes[j].speedY = -shapes[j].speedY;
        }
      }
    }

    if (!anyCollision) break; // если на этой итерации столкновений нет — можно остановиться
  }
}


void drawFrame() {
  drawShape.clearScreen();
  for (int i = 0; i < shapeCount; ++i) {
    drawShape.drawFrame(shapes[i]);
  }
  drawShape.dispCord(shapes[0]);
  drawShape.show();
}