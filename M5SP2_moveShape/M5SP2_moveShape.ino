#include "mv.h"

DrawShape drawShape(SCREEN_WIDTH, SCREEN_HEIGHT);

Shape shapes[MAX_SHAPES] = {
  Shape(10, 30, 2, 3, 12),
  Shape(80, 60, 2, 3, 14),
  Shape(100, 100, 2, 1, 16),
  Shape(60, 120, -1, -1, 10),
};

Wall walls[MAX_WALLS] = {
  Wall(60, 16, 4, 40),
  Wall(60, SCREEN_HEIGHT - 60, 4, 40)
};

int wallCount = 2;
int shapeCount = 4;

// Контроль FPS
unsigned long previousDrawTime = 0;
const int FRAME_DELAY_MS = 1000 / 30;  // 30 FPS — идеальный баланс между плавностью и нагрузкой

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
}

void loop() {
  M5.update();  // Обязательно — для работы кнопок

  unsigned long currentTime = millis();

  // Обновляем логику и рисуем только с нужной частотой
  if (currentTime - previousDrawTime >= FRAME_DELAY_MS) {
    Logic_1();
    drawFrame();
    previousDrawTime = currentTime;
  }

  // Кнопка A — перезапуск позиций
  if (M5.BtnA.wasPressed()) {
    for (int i = 0; i < shapeCount; ++i) {
      shapes[i].xPos = random(10, SCREEN_WIDTH - shapes[i].sizeShape - 10);
      shapes[i].yPos = random(MIN_YPOS, SCREEN_HEIGHT - shapes[i].sizeShape - 10);
    }
  }
}
