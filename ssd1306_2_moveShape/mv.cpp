#include "mv.h"

// Настройки фигур 
#define CENTER_Y (SCREEN_HEIGHT / 2) + (MIN_YPOS / 2)
#define CENTER_X SCREEN_WIDTH / 2
#define SIZE_COLOMN 4

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 20, 2, 3, 6),
  Shape(80, 40, 2, 3, 6),
  Shape(100, 40, 2, 1, 10),
  Shape(60, 30, -1, -1, 10),
};

// Пример: три внутренние стены
Wall walls[MAX_WALLS] = {
  Wall(0, CENTER_Y, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X, CENTER_Y, SIZE_COLOMN, SIZE_COLOMN),
  Wall(SCREEN_WIDTH - SIZE_COLOMN, CENTER_Y, SIZE_COLOMN, SIZE_COLOMN)     
};

int wallCount = 3;

int shapeCount = 4;
// 5 итераций обычно достаточно, чтобы фигуры «расцепились» без лишней нагрузки
const int COLLISION_ITERATIONS = 5;

void Logic_1() {
  // 1. Коллизии между фигурами (твоя стабильная логика)
  predictAndResolveCollisions();

  // 2. Применяем исправленные позиции
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].xPos = shapes[i].nextX;
    shapes[i].yPos = shapes[i].nextY;
  }

  // 3. Коллизии со стенами (новые внутренние препятствия)
  for (int i = 0; i < shapeCount; ++i) {
    for (int j = 0; j < wallCount; ++j) {
      checkShapeWallCollision(shapes[i], walls[j]);
    }
  }

  // 4. Границы экрана (как было)
  for (int i = 0; i < shapeCount; ++i) {
    constrainToWalls(shapes[i]);
  }
}


void constrainToWalls(Shape& s) {
  // Левая стена
  if (s.xPos <= MIN_XPOS) {
    s.xPos = MIN_XPOS;
    // Инвертируем скорость только если летели в стену (защита от «залипания на границе»)
    if (s.speedX < 0) {
      s.speedX = -s.speedX;
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

  // Итеративно корректируем будущие позиции, чтобы не было пересечений
  for (int iter = 0; iter < COLLISION_ITERATIONS; ++iter) {
    bool anyCollision = false;

    for (int i = 0; i < shapeCount; ++i) {
      for (int j = i + 1; j < shapeCount; ++j) {
        bool xOverlap = shapes[i].nextX < shapes[j].nextX + shapes[j].sizeShape
                     && shapes[j].nextX < shapes[i].nextX + shapes[i].sizeShape;
        bool yOverlap = shapes[i].nextY < shapes[j].nextY + shapes[j].sizeShape
                     && shapes[j].nextY < shapes[i].nextY + shapes[i].sizeShape;

        if (!xOverlap || !yOverlap) continue;

        anyCollision = true;

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

        // Коррекция по оси с меньшим перекрытием
        if (overlapX < overlapY) {
          if (shapes[i].nextX < shapes[j].nextX) {
            int contactX = shapes[j].nextX - shapes[i].sizeShape;
            shapes[i].nextX = contactX;
            shapes[j].nextX = shapes[i].nextX + shapes[i].sizeShape;
          } else {
            int contactX = shapes[i].nextX - shapes[j].sizeShape;
            shapes[j].nextX = contactX;
            shapes[i].nextX = shapes[j].nextX + shapes[j].sizeShape;
          }
          shapes[i].speedX = -shapes[i].speedX;
          shapes[j].speedX = -shapes[j].speedX;
        } else {
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

    // Если на этой итерации столкновений нет — можно остановиться раньше
    if (!anyCollision) break;
  }
}

void drawFrame() {
  drawShape.clearScreen();

  // Сначала рисуем стены (чтобы фигуры были «поверх»)
  for (int i = 0; i < wallCount; ++i) {
    drawShape.drawRect(walls[i].x, walls[i].y, walls[i].w, walls[i].h, WHITE);
  }

  // Потом фигуры
  for (int i = 0; i < shapeCount; ++i) {
    drawShape.drawFrame(shapes[i]);
  }

  drawShape.dispCord(shapes[0]);
  drawShape.show();
}


void checkShapeWallCollision(Shape& s, const Wall& w) {
  // Проверка пересечения прямоугольников
  bool xOverlap = s.xPos < w.x + w.w && w.x < s.xPos + s.sizeShape;
  bool yOverlap = s.yPos < w.y + w.h && w.y < s.yPos + s.sizeShape;

  if (!xOverlap || !yOverlap) return;

  // Глубина перекрытия
  int overlapX = 0;
  if (s.xPos < w.x) {
    overlapX = (s.xPos + s.sizeShape) - w.x;
  } else {
    overlapX = (w.x + w.w) - s.xPos;
  }

  int overlapY = 0;
  if (s.yPos < w.y) {
    overlapY = (s.yPos + s.sizeShape) - w.y;
  } else {
    overlapY = (w.y + w.h) - s.yPos;
  }

  // Выталкиваем по оси с меньшим перекрытием
  if (overlapX < overlapY) {
    // Столкновение по X: корректируем X, инвертируем speedX
    if (s.xPos < w.x) {
      s.xPos = w.x - s.sizeShape; // фигура слева, ставим вплотную к стене
    } else {
      s.xPos = w.x + w.w;          // фигура справа, ставим вплотную
    }
    s.speedX = -s.speedX;
  } else {
    // Столкновение по Y: корректируем Y, инвертируем speedY
    if (s.yPos < w.y) {
      s.yPos = w.y - s.sizeShape;
    } else {
      s.yPos = w.y + w.h;
    }
    s.speedY = -s.speedY;
  }
}

