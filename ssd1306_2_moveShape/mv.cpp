#include "mv.h"

// Настройки фигур 
#define CENTER_Y (SCREEN_HEIGHT / 2) + (MIN_YPOS / 2)
#define CENTER_X SCREEN_WIDTH / 2
#define SIZE_COLOMN 3

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 20, 2, 3, 6),
  Shape(80, 40, 2, 3, 6),
  Shape(100, 40, 2, 1, 8),
  Shape(60, 30, -1, -1, 8),
};

Wall walls[MAX_WALLS] = {
  Wall(CENTER_X, CENTER_Y, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X / 2, SCREEN_HEIGHT - SIZE_COLOMN, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X + CENTER_X / 2, MIN_YPOS, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X / 2, MIN_YPOS, SIZE_COLOMN, SIZE_COLOMN),
  Wall(CENTER_X + CENTER_X / 2, SCREEN_HEIGHT - SIZE_COLOMN, SIZE_COLOMN, SIZE_COLOMN)     
};

int wallCount = 5;
int shapeCount = 4;
const int COLLISION_ITERATIONS = 5;

void Logic_1() {
  // 1. Коллизии между фигурами
  predictAndResolveCollisions();

  // 2. Применяем исправленные позиции
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].xPos = shapes[i].nextX;
    shapes[i].yPos = shapes[i].nextY;
  }

  // 3. Итеративная обработка столкновений со стенами (исправленная версия)
  for (int i = 0; i < shapeCount; ++i) {
    bool anyCollision = true;
    int iter = 0;
    // Повторяем, пока есть пересечения, но не более 10 раз (защита от зацикливания)
    while (anyCollision && iter < 10) {
      anyCollision = false;
      for (int j = 0; j < wallCount; ++j) {
        // checkShapeWallCollision возвращает true, если было разрешено столкновение
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
}

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

        // Симметричное выталкивание
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
      if (s.speedX < 0) s.speedX = -s.speedX;   // инвертируем, если движемся к стене
    } else {
      s.xPos = w.x + w.w;
      if (s.speedX > 0) s.speedX = -s.speedX;
    }
  } else {
    if (s.yPos < w.y) {
      s.yPos = w.y - s.sizeShape;
      if (s.speedY < 0) s.speedY = -s.speedY;
    } else {
      s.yPos = w.y + w.h;
      if (s.speedY > 0) s.speedY = -s.speedY;
    }
  }
  return true;
}

void drawFrame() {
  drawShape.clearScreen();

  for (int i = 0; i < wallCount; ++i) {
    drawShape.drawRect(walls[i].x, walls[i].y, walls[i].w, walls[i].h, WHITE);
  }

  for (int i = 0; i < shapeCount; ++i) {
    drawShape.drawFrame(shapes[i]);
  }

  drawShape.dispCord(shapes[0]);
  drawShape.show();
}