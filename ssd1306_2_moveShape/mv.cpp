#include "mv.h"

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 20, 1, 1, 6, SHAPEMASS_LIGHT),
  Shape(100, 40, 1, 1, 8, SHAPEMASS_MID),
  Shape(60, 30, 1, 1, 10, SHAPEMASS_HEAVY),
};

int shapeCount = 3;
const int COLLISION_ITERATIONS = 30;   // увеличено для надёжности

// ------------------- Логика -------------------
void Logic_1() {
  // Движение
  for (int i = 0; i < shapeCount; ++i) {
    shapes[i].xPos += shapes[i].speedX;
    shapes[i].yPos += shapes[i].speedY;
  }

  // Многократное разрешение столкновений
  for (int iter = 0; iter < COLLISION_ITERATIONS; ++iter) {
    // Столкновения между фигурами
    for (int i = 0; i < shapeCount; ++i) {
      for (int j = i + 1; j < shapeCount; ++j) {
        resolveCollision(shapes[i], shapes[j]);
      }
    }
    // Стены после каждой итерации
    for (int i = 0; i < shapeCount; ++i) {
      constrainToWalls(shapes[i]);
    }
  }
}

// ------------------- Стены (исправлены нестрогие сравнения) -------------------
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

// ------------------- Столкновение двух фигур (улучшенное) -------------------
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

  // Защита от нулевой скорости
  if (a.speedX == 0 && oldAX != 0) a.speedX = (oldAX > 0) ? 1 : -1;
  if (b.speedX == 0 && oldBX != 0) b.speedX = (oldBX > 0) ? 1 : -1;
  if (a.speedY == 0 && oldAY != 0) a.speedY = (oldAY > 0) ? 1 : -1;
  if (b.speedY == 0 && oldBY != 0) b.speedY = (oldBY > 0) ? 1 : -1;

  // ---- Выталкивание: выбираем ось с меньшим перекрытием ----
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

  // Если перекрытие по X меньше (или равно) – раздвигаем по X, иначе по Y
  if (overlapX <= overlapY) {
    // Раздвигаем по X
    if (a.xPos < b.xPos) {
      // a слева от b
      int maxShiftA = a.xPos - MIN_XPOS;                     // сколько можно сдвинуть a влево
      int maxShiftB = (SCREEN_WIDTH - b.sizeShape) - b.xPos; // сколько можно сдвинуть b вправо
      int totalOverlap = overlapX;
      int shiftA = constrain(totalOverlap / 2, 0, maxShiftA);
      int shiftB = totalOverlap - shiftA;
      if (shiftB > maxShiftB) {
        shiftB = maxShiftB;
        shiftA = totalOverlap - shiftB;
        if (shiftA > maxShiftA) shiftA = maxShiftA;
      }
      a.xPos -= shiftA;
      b.xPos += shiftB;
    } else {
      // b слева от a
      int maxShiftA = (SCREEN_WIDTH - a.sizeShape) - a.xPos;
      int maxShiftB = b.xPos - MIN_XPOS;
      int totalOverlap = overlapX;
      int shiftA = constrain(totalOverlap / 2, 0, maxShiftA);
      int shiftB = totalOverlap - shiftA;
      if (shiftB > maxShiftB) {
        shiftB = maxShiftB;
        shiftA = totalOverlap - shiftB;
        if (shiftA > maxShiftA) shiftA = maxShiftA;
      }
      a.xPos += shiftA;
      b.xPos -= shiftB;
    }
  } else {
    // Раздвигаем по Y
    if (a.yPos < b.yPos) {
      int maxShiftA = a.yPos - MIN_YPOS;
      int maxShiftB = (SCREEN_HEIGHT - b.sizeShape) - b.yPos;
      int totalOverlap = overlapY;
      int shiftA = constrain(totalOverlap / 2, 0, maxShiftA);
      int shiftB = totalOverlap - shiftA;
      if (shiftB > maxShiftB) {
        shiftB = maxShiftB;
        shiftA = totalOverlap - shiftB;
        if (shiftA > maxShiftA) shiftA = maxShiftA;
      }
      a.yPos -= shiftA;
      b.yPos += shiftB;
    } else {
      int maxShiftA = (SCREEN_HEIGHT - a.sizeShape) - a.yPos;
      int maxShiftB = b.yPos - MIN_YPOS;
      int totalOverlap = overlapY;
      int shiftA = constrain(totalOverlap / 2, 0, maxShiftA);
      int shiftB = totalOverlap - shiftA;
      if (shiftB > maxShiftB) {
        shiftB = maxShiftB;
        shiftA = totalOverlap - shiftB;
        if (shiftA > maxShiftA) shiftA = maxShiftA;
      }
      a.yPos += shiftA;
      b.yPos -= shiftB;
    }
  }

  // ... (весь код расчёта скоростей и выталкивания)

  // ---- Шум для разрушения заторов ----
  // С вероятностью 50% сдвигаем каждую фигуру на -1, 0 или +1 пиксель
  if (random(0, 2)) { 
    a.xPos += random(-1, 2); 
    b.xPos += random(-1, 2); 
  }
  if (random(0, 2)) { 
    a.yPos += random(-1, 2); 
    b.yPos += random(-1, 2); 
  }

  // ---- Финальная коррекция стен ----
  constrainToWalls(a);
  constrainToWalls(b);
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
