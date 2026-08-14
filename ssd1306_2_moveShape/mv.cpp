#include "mv.h"

// Настройки фигур 
#define CENTER_Y (SCREEN_HEIGHT / 2) + (MIN_YPOS / 2)
#define CENTER_X SCREEN_WIDTH / 2
#define SIZE_COLOMN 3

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 20, 2, 3, 6),
  Shape(80, 40, 2, 3, 6),
  Shape(100, 40, 2, 1, 6),
  Shape(60, 30, -1, -1, 6),
  Shape(90, 20, 2, -1, 6),
  Shape(40, 40, -1, -1, 6),
};

Wall walls[MAX_WALLS] = {
  Wall(CENTER_X, CENTER_Y, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X / 2, SCREEN_HEIGHT - SIZE_COLOMN, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X + CENTER_X / 2, MIN_YPOS, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X / 2, MIN_YPOS, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X + CENTER_X / 2, SCREEN_HEIGHT - SIZE_COLOMN, SIZE_COLOMN, SIZE_COLOMN)     
};

int wallCount = 5;
int shapeCount = 6;
const int COLLISION_ITERATIONS = 5;

// ============================================================
// ОСНОВНАЯ ЛОГИКА
// ============================================================
void Logic_1() {
  // 1. Коллизии между фигурами (предсказание + итеративное разрешение)
  predictAndResolveCollisions();

  // 2. Применяем исправленные позиции
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].xPos = shapes[i].nextX;
    shapes[i].yPos = shapes[i].nextY;
  }

  // 3. Итеративная обработка столкновений со стенами (увеличено до 20 итераций)
  for (int i = 0; i < shapeCount; ++i) {
    bool anyCollision = true;
    int iter = 0;
    while (anyCollision && iter < 20) {   // было 10
      anyCollision = false;
      for (int j = 0; j < wallCount; ++j) {
        if (checkShapeWallCollision(shapes[i], walls[j])) {
          anyCollision = true;
        }
      }
      iter++;
    }
  }

  // 4. Границы экрана
  for (int i = 0; i < shapeCount; ++i) {
    constrainToWalls(shapes[i]);
  }

  // 5. Повторное разрешение коллизий между фигурами (финальный проход)
  resolveFinalShapeCollisions();
}

// ============================================================
// Финальное разрешение коллизий между фигурами
// ============================================================
void resolveFinalShapeCollisions() {
  const int FINAL_ITERATIONS = 3;
  for (int iter = 0; iter < FINAL_ITERATIONS; ++iter) {
    bool anyCollision = false;
    for (int i = 0; i < shapeCount; ++i) {
      for (int j = i + 1; j < shapeCount; ++j) {
        bool xOverlap = shapes[i].xPos < shapes[j].xPos + shapes[j].sizeShape &&
                        shapes[j].xPos < shapes[i].xPos + shapes[i].sizeShape;
        bool yOverlap = shapes[i].yPos < shapes[j].yPos + shapes[j].sizeShape &&
                        shapes[j].yPos < shapes[i].yPos + shapes[i].sizeShape;
        if (xOverlap && yOverlap) {
          anyCollision = true;
          resolveCollision(shapes[i], shapes[j]);
        }
      }
    }
    if (!anyCollision) break;
  }
}

// ============================================================
// Ограничение границами экрана (без изменений)
// ============================================================
void constrainToWalls(Shape& s) {
  if (s.xPos <= MIN_XPOS) {
    s.xPos = MIN_XPOS;
    if (s.speedX < 0) s.speedX = -s.speedX;
  }
  if (s.xPos + s.sizeShape >= SCREEN_WIDTH) {
    s.xPos = SCREEN_WIDTH - s.sizeShape;
    if (s.speedX > 0) s.speedX = -s.speedX;
  }
  if (s.yPos <= MIN_YPOS) {
    s.yPos = MIN_YPOS;
    if (s.speedY < 0) s.speedY = -s.speedY;
  }
  if (s.yPos + s.sizeShape >= SCREEN_HEIGHT) {
    s.yPos = SCREEN_HEIGHT - s.sizeShape;
    if (s.speedY > 0) s.speedY = -s.speedY;
  }
}

// ============================================================
// Разрешение столкновения между двумя фигурами (без изменений)
// ============================================================
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

// ============================================================
// predictAndResolveCollisions (симметричное выталкивание, без изменений)
// ============================================================
void predictAndResolveCollisions() {
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].nextX = shapes[i].xPos + shapes[i].speedX;
    shapes[i].nextY = shapes[i].yPos + shapes[i].speedY;
  }

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

        if (overlapX < overlapY) {
          int shift = overlapX / 2;
          if (shapes[i].nextX < shapes[j].nextX) {
            shapes[i].nextX -= shift;
            shapes[j].nextX += shift;
          } else {
            shapes[i].nextX += shift;
            shapes[j].nextX -= shift;
          }
          shapes[i].speedX = -shapes[i].speedX;
          shapes[j].speedX = -shapes[j].speedX;
        } else {
          int shift = overlapY / 2;
          if (shapes[i].nextY < shapes[j].nextY) {
            shapes[i].nextY -= shift;
            shapes[j].nextY += shift;
          } else {
            shapes[i].nextY += shift;
            shapes[j].nextY -= shift;
          }
          shapes[i].speedY = -shapes[i].speedY;
          shapes[j].speedY = -shapes[j].speedY;
        }
      }
    }

    if (!anyCollision) break;
  }
}

// ============================================================
// checkShapeWallCollision – ИСПРАВЛЕНА (убрана проверка направления скорости)
// ============================================================
bool checkShapeWallCollision(Shape& s, const Wall& w) {
  bool xOverlap = s.xPos < w.x + w.w && w.x < s.xPos + s.sizeShape;
  bool yOverlap = s.yPos < w.y + w.h && w.y < s.yPos + s.sizeShape;
  if (!xOverlap || !yOverlap) return false;

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

  if (overlapX < overlapY) {
    if (s.xPos < w.x) {
      s.xPos = w.x - s.sizeShape;
    } else {
      s.xPos = w.x + w.w;
    }
    // Всегда инвертируем скорость – так фигура гарантированно отскочит
    s.speedX = -s.speedX;
  } else {
    if (s.yPos < w.y) {
      s.yPos = w.y - s.sizeShape;
    } else {
      s.yPos = w.y + w.h;
    }
    s.speedY = -s.speedY;
  }
  return true;
}

// ============================================================
// Отрисовка
// ============================================================
void drawFrame() {
  drawShape.clearScreen();

  for (int i = 0; i < wallCount; ++i) {
    drawShape.drawRect(walls[i].x, walls[i].y, walls[i].w, walls[i].h, WHITE);
  }

  for (int i = 0; i < shapeCount; ++i) {
    drawShape.drawFrame(shapes[i]);
  }

  drawShape.show();
}