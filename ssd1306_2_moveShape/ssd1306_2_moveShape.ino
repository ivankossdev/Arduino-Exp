#include "mv.h"

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 20, 1, 1, 6, SHAPEMASS_LIGHT),
  Shape(100, 40, 1, 1, 8, SHAPEMASS_MED),
  Shape(60, 30, 1, 1, 10, SHAPEMASS_HEAVY),
};


int shapeCount = 2;

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
    drawShape.drawFrame(shapes[i]);
  }

  drawShape.dispCord(shapes[0]);  // координаты первой фигуры (или любой другой)
  drawShape.show();
}


void updateShape(Shape& s) {
  s.xPos += s.speedX;
  s.yPos += s.speedY;

  // Отскок от левой/правой
  if (s.xPos <= MIN_XPOS) {
    s.xPos = MIN_XPOS;
    // После отскока скорость должна быть равна базовой по модулю, но вправо
    s.speedX = abs(s.baseSpeedX);
  } else if (s.xPos + s.sizeShape >= SCREEN_WIDTH) {
    s.xPos = MAX_XPOS(s);
    // После отскока — влево, с базовой скоростью
    s.speedX = -abs(s.baseSpeedX);
  }

  // Отскок от верхней/нижней
  if (s.yPos <= MIN_YPOS) {
    s.yPos = MIN_YPOS;
    s.speedY = abs(s.baseSpeedY);
  } else if (s.yPos + s.sizeShape >= SCREEN_HEIGHT) {
    s.yPos = MAX_YPOS(s);
    s.speedY = -abs(s.baseSpeedY);
  }

}

void checkCollisionBetween(Shape& a, Shape& b) {
  bool xOverlap = a.xPos < b.xPos + b.sizeShape && b.xPos < a.xPos + a.sizeShape;

  bool yOverlap = a.yPos < b.yPos + b.sizeShape && b.yPos < a.yPos + a.sizeShape;

  if (xOverlap && yOverlap) {

    // 1. Запоминаем старые скорости ДО изменения
    int16_t oldAX = a.speedX;
    int16_t oldBX = b.speedX;
    int16_t oldAY = a.speedY;
    int16_t oldBY = b.speedY;

    // 2. Считаем новые скорости в int32_t
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

    // 3. ЗАЩИТА ОТ ЗАМЕРЗАНИЯ (сразу после присваивания)
    auto protectSpeed = [](int16_t& v, int16_t oldV, int16_t baseV) {
      if (oldV != 0 && v == 0) {
        v = (oldV > 0) ? 1 : -1;
      }
    };

    protectSpeed(a.speedX, oldAX, a.baseSpeedX);
    protectSpeed(b.speedX, oldBX, b.baseSpeedX);
    protectSpeed(a.speedY, oldAY, a.baseSpeedY);
    protectSpeed(b.speedY, oldBY, b.baseSpeedY);

    // 4. Выталкивание, чтобы не залипли
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
