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
  // Движение
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].xPos += shapes[i].speedX;
    shapes[i].yPos += shapes[i].speedY;
  }

  // Многократное разрешение столкновений
  for (int iter = 0; iter < COLLISION_ITERATIONS; ++iter) {
    for (int i = 0; i < shapeCount; ++i) {
      for (int j = i + 1; j < shapeCount; ++j) {
        resolveCollision(shapes[i], shapes[j]);
      }
    }
    for (int i = 0; i < shapeCount; ++i) {
      constrainToWalls(shapes[i]);
    }
  }
}

void constrainToWalls(Shape& s) {
  if (s.xPos <= MIN_XPOS) {
    s.xPos = MIN_XPOS;
    if (s.speedX < 0) s.speedX = -s.speedX;
    else if (s.speedX == 0) s.speedX = 1;
  }
  if (s.xPos + s.sizeShape >= SCREEN_WIDTH) {
    s.xPos = SCREEN_WIDTH - s.sizeShape;
    if (s.speedX > 0) s.speedX = -s.speedX;
    else if (s.speedX == 0) s.speedX = -1;
  }

  if (s.yPos <= MIN_YPOS) {
    s.yPos = MIN_YPOS;
    if (s.speedY < 0) s.speedY = -s.speedY;
    else if (s.speedY == 0) s.speedY = 1;
  }
  if (s.yPos + s.sizeShape >= SCREEN_HEIGHT) {
    s.yPos = SCREEN_HEIGHT - s.sizeShape;
    if (s.speedY > 0) s.speedY = -s.speedY;
    else if (s.speedY == 0) s.speedY = -1;
  }
}

void resolveCollision(Shape& a, Shape& b) {
    // Проверка пересечения
    bool xOverlap = a.xPos < b.xPos + b.sizeShape && b.xPos < a.xPos + a.sizeShape;
    bool yOverlap = a.yPos < b.yPos + b.sizeShape && b.yPos < a.yPos + a.sizeShape;
    if (!xOverlap || !yOverlap) return;

    // Глубина перекрытия по X
    int overlapX = 0;
    if (a.xPos < b.xPos) {
        overlapX = (a.xPos + a.sizeShape) - b.xPos;
    } else {
        overlapX = (b.xPos + b.sizeShape) - a.xPos;
    }
    // Глубина перекрытия по Y
    int overlapY = 0;
    if (a.yPos < b.yPos) {
        overlapY = (a.yPos + a.sizeShape) - b.yPos;
    } else {
        overlapY = (b.yPos + b.sizeShape) - a.yPos;
    }

    // Выбираем ось с бóльшим перекрытием
    if (overlapX >= overlapY) {
        // Отражаем скорости по X
        a.speedX = -a.speedX;
        b.speedX = -b.speedX;

        // Выталкивание по X
        if (a.xPos < b.xPos) {
            int maxShiftA = a.xPos - MIN_XPOS;
            int maxShiftB = (SCREEN_WIDTH - b.sizeShape) - b.xPos;
            int shiftA = constrain(overlapX / 2, 0, maxShiftA);
            int shiftB = overlapX - shiftA;
            if (shiftB > maxShiftB) { shiftB = maxShiftB; shiftA = overlapX - shiftB; }
            if (shiftA > maxShiftA) shiftA = maxShiftA;
            a.xPos -= shiftA;
            b.xPos += shiftB;
        } else {
            int maxShiftA = (SCREEN_WIDTH - a.sizeShape) - a.xPos;
            int maxShiftB = b.xPos - MIN_XPOS;
            int shiftA = constrain(overlapX / 2, 0, maxShiftA);
            int shiftB = overlapX - shiftA;
            if (shiftB > maxShiftB) { shiftB = maxShiftB; shiftA = overlapX - shiftB; }
            if (shiftA > maxShiftA) shiftA = maxShiftA;
            a.xPos += shiftA;
            b.xPos -= shiftB;
        }
    } else {
        // Отражаем скорости по Y
        a.speedY = -a.speedY;
        b.speedY = -b.speedY;

        // Выталкивание по Y
        if (a.yPos < b.yPos) {
            int maxShiftA = a.yPos - MIN_YPOS;
            int maxShiftB = (SCREEN_HEIGHT - b.sizeShape) - b.yPos;
            int shiftA = constrain(overlapY / 2, 0, maxShiftA);
            int shiftB = overlapY - shiftA;
            if (shiftB > maxShiftB) { shiftB = maxShiftB; shiftA = overlapY - shiftB; }
            if (shiftA > maxShiftA) shiftA = maxShiftA;
            a.yPos -= shiftA;
            b.yPos += shiftB;
        } else {
            int maxShiftA = (SCREEN_HEIGHT - a.sizeShape) - a.yPos;
            int maxShiftB = b.yPos - MIN_YPOS;
            int shiftA = constrain(overlapY / 2, 0, maxShiftA);
            int shiftB = overlapY - shiftA;
            if (shiftB > maxShiftB) { shiftB = maxShiftB; shiftA = overlapY - shiftB; }
            if (shiftA > maxShiftA) shiftA = maxShiftA;
            a.yPos += shiftA;
            b.yPos -= shiftB;
        }
    }

    // Защита от нулевой скорости
    if (a.speedX == 0) a.speedX = random(0, 2) ? 1 : -1;
    if (b.speedX == 0) b.speedX = random(0, 2) ? 1 : -1;
    if (a.speedY == 0) a.speedY = random(0, 2) ? 1 : -1;
    if (b.speedY == 0) b.speedY = random(0, 2) ? 1 : -1;

    // Финальная коррекция стен
    constrainToWalls(a);
    constrainToWalls(b);
}

void drawFrame() {
  drawShape.clearScreen();
  for (int i = 0; i < shapeCount; ++i) {
    drawShape.drawFrame(shapes[i]);
  }
  drawShape.dispCord(shapes[0]);
  drawShape.show();
}