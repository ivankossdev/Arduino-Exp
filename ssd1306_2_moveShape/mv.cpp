#include "mv.h"

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 20, 1, 1, 6, SHAPEMASS_LIGHT),
  Shape(100, 40, 1, 1, 8, SHAPEMASS_MED),
  // Shape(60, 30, 1, 1, 10, SHAPEMASS_HEAVY),
};

int shapeCount = 2;  // теперь все три

// ------------------- Новая логика -------------------
void Logic_1() {
  // 1. Движение
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].xPos += shapes[i].speedX;
    shapes[i].yPos += shapes[i].speedY;
  }

  // 2. Столкновения между фигурами (разрешение)
  for (int i = 0; i < shapeCount; ++i) {
    for (int j = i + 1; j < shapeCount; ++j) {
      resolveCollision(shapes[i], shapes[j]);
    }
  }

  // 3. Столкновения со стенами (после разделения)
  for (int i = 0; i < shapeCount; ++i) {
    constrainToWalls(shapes[i]);
  }
}

// ------------------- Стены -------------------
void constrainToWalls(Shape& s) {
  // Левая стена
  if (s.xPos < MIN_XPOS) {
    s.xPos = MIN_XPOS;
    s.speedX = -s.speedX;          // инвертируем текущую скорость
  }
  // Правая стена
  else if (s.xPos + s.sizeShape > SCREEN_WIDTH) {
    s.xPos = SCREEN_WIDTH - s.sizeShape;
    s.speedX = -s.speedX;
  }

  // Верхняя стена (с учётом MIN_YPOS)
  if (s.yPos < MIN_YPOS) {
    s.yPos = MIN_YPOS;
    s.speedY = -s.speedY;
  }
  // Нижняя стена
  else if (s.yPos + s.sizeShape > SCREEN_HEIGHT) {
    s.yPos = SCREEN_HEIGHT - s.sizeShape;
    s.speedY = -s.speedY;
  }
}

// ------------------- Столкновение двух фигур -------------------
void resolveCollision(Shape& a, Shape& b) {
  // Проверка пересечения
  bool xOverlap = a.xPos < b.xPos + b.sizeShape && b.xPos < a.xPos + a.sizeShape;
  bool yOverlap = a.yPos < b.yPos + b.sizeShape && b.yPos < a.yPos + a.sizeShape;
  if (!xOverlap || !yOverlap) return;

  // ---- Расчёт новых скоростей (упругое столкновение) ----
  int16_t oldAX = a.speedX, oldBX = b.speedX;
  int16_t oldAY = a.speedY, oldBY = b.speedY;

  int32_t sumMass = (int32_t)a.mass + b.mass;
  if (sumMass == 0) sumMass = 1;

  int32_t newSpeedAX32 = ((int32_t)a.speedX * (a.mass - b.mass) + 2L * b.mass * b.speedX) / sumMass;
  int32_t newSpeedBX32 = ((int32_t)b.speedX * (b.mass - a.mass) + 2L * a.mass * a.speedX) / sumMass;
  int32_t newSpeedAY32 = ((int32_t)a.speedY * (a.mass - b.mass) + 2L * b.mass * b.speedY) / sumMass;
  int32_t newSpeedBY32 = ((int32_t)b.speedY * (b.mass - a.mass) + 2L * a.mass * a.speedY) / sumMass;

  a.speedX = (int16_t)newSpeedAX32;
  b.speedX = (int16_t)newSpeedBX32;
  a.speedY = (int16_t)newSpeedAY32;
  b.speedY = (int16_t)newSpeedBY32;

  // Защита от нулевой скорости (если обе скорости стали нулевыми – задаём единицу)
  if (a.speedX == 0 && oldAX != 0) a.speedX = (oldAX > 0) ? 1 : -1;
  if (b.speedX == 0 && oldBX != 0) b.speedX = (oldBX > 0) ? 1 : -1;
  if (a.speedY == 0 && oldAY != 0) a.speedY = (oldAY > 0) ? 1 : -1;
  if (b.speedY == 0 && oldBY != 0) b.speedY = (oldBY > 0) ? 1 : -1;

  // ---- Выталкивание с учётом глубины перекрытия (поровну) ----
  int overlapX = 0;
  if (a.xPos < b.xPos) {
    overlapX = (a.xPos + a.sizeShape) - b.xPos;
    if (overlapX > 0) {
      a.xPos -= overlapX / 2;
      b.xPos += overlapX / 2;
      // если нечётное – один пиксель отдаём a
      if (overlapX % 2 != 0) a.xPos -= 1;
    }
  } else {
    overlapX = (b.xPos + b.sizeShape) - a.xPos;
    if (overlapX > 0) {
      a.xPos += overlapX / 2;
      b.xPos -= overlapX / 2;
      if (overlapX % 2 != 0) a.xPos += 1;
    }
  }

  int overlapY = 0;
  if (a.yPos < b.yPos) {
    overlapY = (a.yPos + a.sizeShape) - b.yPos;
    if (overlapY > 0) {
      a.yPos -= overlapY / 2;
      b.yPos += overlapY / 2;
      if (overlapY % 2 != 0) a.yPos -= 1;
    }
  } else {
    overlapY = (b.yPos + b.sizeShape) - a.yPos;
    if (overlapY > 0) {
      a.yPos += overlapY / 2;
      b.yPos -= overlapY / 2;
      if (overlapY % 2 != 0) a.yPos += 1;
    }
  }

  // Дополнительная страховка: если всё ещё пересекаются – раздвигаем принудительно
  if (a.xPos < b.xPos + b.sizeShape && b.xPos < a.xPos + a.sizeShape) {
    if (a.xPos < b.xPos) {
      a.xPos -= 1;
      b.xPos += 1;
    } else {
      a.xPos += 1;
      b.xPos -= 1;
    }
  }
  if (a.yPos < b.yPos + b.sizeShape && b.yPos < a.yPos + a.sizeShape) {
    if (a.yPos < b.yPos) {
      a.yPos -= 1;
      b.yPos += 1;
    } else {
      a.yPos += 1;
      b.yPos -= 1;
    }
  }
}

// ------------------- Отрисовка -------------------
void drawFrame() {
  drawShape.clearScreen();
  for (int i = 0; i < shapeCount; ++i) {
    drawShape.drawFrame(shapes[i]);
  }
  drawShape.dispCord(shapes[0]);
  drawShape.show();
}
