#include "mv.h"

void Logic_1() {
  // Сбрасываем флаги
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].isColliding = false;
  }

  predictAndResolveCollisions();

  // Сохраняем старые позиции ДО обновления
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].prevX = shapes[i].xPos;
    shapes[i].prevY = shapes[i].yPos;
  }

  // Обновляем позиции
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].xPos = shapes[i].nextX;
    shapes[i].yPos = shapes[i].nextY;
  }

  // Остальная логика...
  for (int i = 0; i < shapeCount; ++i) {
    for (int j = 0; j < wallCount; ++j) {
      checkShapeWallCollision(shapes[i], walls[j]);
    }
  }

  for (int i = 0; i < shapeCount; ++i) {
    constrainToWalls(shapes[i]);
  }
}

// void Logic_1() {
//   predictAndResolveCollisions();

//   for (int i = 0; i < shapeCount; ++i) {
//     shapes[i].xPos = shapes[i].nextX;
//     shapes[i].yPos = shapes[i].nextY;
//   }

//   for (int i = 0; i < shapeCount; ++i) {
//     for (int j = 0; j < wallCount; ++j) {
//       checkShapeWallCollision(shapes[i], walls[j]);
//     }
//   }

//   for (int i = 0; i < shapeCount; ++i) {
//     constrainToWalls(shapes[i]);
//   }
// }

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

  a.isColliding = true;
  b.isColliding = true;

  int overlapX = (a.xPos < b.xPos) ? (a.xPos + a.sizeShape) - b.xPos : (b.xPos + b.sizeShape) - a.xPos;
  int overlapY = (a.yPos < b.yPos) ? (a.yPos + a.sizeShape) - b.yPos : (b.yPos + b.sizeShape) - a.yPos;

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

  // Сбрасываем флаги столкновений перед кадром
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].isColliding = false;
  }

  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].nextX = shapes[i].xPos + shapes[i].speedX;
    shapes[i].nextY = shapes[i].yPos + shapes[i].speedY;
  }

  for (int iter = 0; iter < COLLISION_ITERATIONS; ++iter) {
    bool anyCollision = false;
    for (int i = 0; i < shapeCount; ++i) {
      for (int j = i + 1; j < shapeCount; ++j) {
        bool xOverlap = shapes[i].nextX < shapes[j].nextX + shapes[j].sizeShape && shapes[j].nextX < shapes[i].nextX + shapes[i].sizeShape;
        bool yOverlap = shapes[i].nextY < shapes[j].nextY + shapes[j].sizeShape && shapes[j].nextY < shapes[i].nextY + shapes[i].sizeShape;
        if (!xOverlap || !yOverlap) continue;

        anyCollision = true;

        shapes[i].isColliding = true;
        shapes[j].isColliding = true;

        int overlapX = (shapes[i].nextX < shapes[j].nextX) ? (shapes[i].nextX + shapes[i].sizeShape) - shapes[j].nextX : (shapes[j].nextX + shapes[j].sizeShape) - shapes[i].nextX;

        int overlapY = (shapes[i].nextY < shapes[j].nextY) ? (shapes[i].nextY + shapes[i].sizeShape) - shapes[j].nextY : (shapes[j].nextY + shapes[j].sizeShape) - shapes[i].nextY;

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
    if (!anyCollision) break;
  }
}

void drawFrame() {
  // Рисуем стены
  for (int i = 0; i < wallCount; ++i) {
    drawShape.drawRect(walls[i].x, walls[i].y, walls[i].w, walls[i].h, TFT_WHITE);
  }

  // Для каждой фигуры — стираем весь путь и рисуем новую
  for (int i = 0; i < shapeCount; ++i) {
    Shape& s = shapes[i];

    // Определяем bounding box движения
    int16_t x1 = min(s.prevX, s.xPos);
    int16_t y1 = min(s.prevY, s.yPos);
    int16_t x2 = max(s.prevX + s.sizeShape, s.xPos + s.sizeShape);
    int16_t y2 = max(s.prevY + s.sizeShape, s.yPos + s.sizeShape);

    int16_t w = x2 - x1;
    int16_t h = y2 - y1;

    // Стираем ВСЁ, что могло остаться
    M5.Lcd.fillRect(x1, y1, w, h, TFT_BLACK);

    // Рисуем новую позицию
    drawShape.drawFrame(s);
  }
}


void checkShapeWallCollision(Shape& s, const Wall& w) {
  bool xOverlap = s.xPos < w.x + w.w && w.x < s.xPos + s.sizeShape;
  bool yOverlap = s.yPos < w.y + w.h && w.y < s.yPos + s.sizeShape;
  if (!xOverlap || !yOverlap) return;

  int overlapX = (s.xPos < w.x) ? (s.xPos + s.sizeShape) - w.x : (w.x + w.w) - s.xPos;

  int overlapY = (s.yPos < w.y) ? (s.yPos + s.sizeShape) - w.y : (w.y + w.h) - s.yPos;

  if (overlapX < overlapY) {
    if (s.xPos < w.x) s.xPos = w.x - s.sizeShape;
    else s.xPos = w.x + w.w;
    s.speedX = -s.speedX;
  } else {
    if (s.yPos < w.y) s.yPos = w.y - s.sizeShape;
    else s.yPos = w.y + w.h;
    s.speedY = -s.speedY;
  }
}
