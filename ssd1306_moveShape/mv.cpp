#include "mv.h"

int16_t targetX = -1;
int16_t targetY = -1;


void mshInit() {
  MShape.init();
}

void mvCord(int16_t x, int16_t y) {
  // 1. Устанавливаем координаты
  MShape.xPos = x;
  MShape.yPos = y;

  // 2. Корректируем границы
  if (MShape.xPos > MAX_XPOS) MShape.xPos = MAX_XPOS;
  if (MShape.xPos < MIN_XPOS) MShape.xPos = MIN_XPOS;
  if (MShape.yPos > MAX_YPOS) MShape.yPos = MAX_YPOS;
  if (MShape.yPos < MIN_YPOS) MShape.yPos = MIN_YPOS;

  // 3. Изменяем буфер (стираем старый квадрат, рисуем новый)
  MShape.movement(MShape.xPos, MShape.yPos);  // внутри уже есть clearSh и fillRect

  // 4. Выводим координаты (тоже в буфер)
  MShape.dispCord();

  // 5. Один раз отправляем буфер на экран
  MShape.updateDisplay();
}

void mvRight() {
  while (MShape.xPos < MAX_XPOS) {
    mvCord(MShape.xPos + 1, MShape.yPos);
  }
}

void mvDown() {
  while (MShape.yPos < MAX_YPOS) {
    mvCord(MShape.xPos, MShape.yPos + 1);
  }
}

void mvLeft() {
  while (MShape.xPos > MIN_XPOS) {
    mvCord(MShape.xPos - 1, MShape.yPos);
  }
}

void mvUp() {
  while (MShape.yPos > MIN_YPOS) {
    mvCord(MShape.xPos, MShape.yPos - 1);
  }
}

void downAndRight() {
  while (MShape.yPos < MAX_YPOS && MShape.xPos < MAX_XPOS) {
    mvCord(MShape.xPos + 1, MShape.yPos + 1);
  }
}

void upAndRight() {  // исправлено имя
  while (MShape.yPos > MIN_YPOS && MShape.xPos < MAX_XPOS) {
    mvCord(MShape.xPos + 1, MShape.yPos - 1);
  }
}

void downAndLeft() {
  while (MShape.yPos < MAX_YPOS && MShape.xPos > MIN_XPOS) {
    mvCord(MShape.xPos - 1, MShape.yPos + 1);
  }
}

void upAndLeft() {
  while (MShape.yPos > MIN_YPOS && MShape.xPos > MIN_XPOS) {
    mvCord(MShape.xPos - 1, MShape.yPos - 1);
  }
}

void clear() {
  MShape.clear();          // закрашиваем текущий квадрат чёрным
  MShape.updateDisplay();  // обновляем экран
}

void stepTowardsTarget() {
  if (targetX == -1 || targetY == -1) {
    return;  // нет цели — ничего не делаем
  }

  bool moved = false;

  // Двигаем X
  if (MShape.xPos < targetX) {
    MShape.xPos += STEP_SIZE;
    moved = true;
  } else if (MShape.xPos > targetX) {
    MShape.xPos -= STEP_SIZE;
    moved = true;
  }

  // Двигаем Y
  if (MShape.yPos < targetY) {
    MShape.yPos += STEP_SIZE;
    moved = true;
  } else if (MShape.yPos > targetY) {
    MShape.yPos -= STEP_SIZE;
    moved = true;
  }

  // Корректируем границы (на всякий случай)
  if (MShape.xPos > MAX_XPOS) MShape.xPos = MAX_XPOS;
  if (MShape.xPos < MIN_XPOS) MShape.xPos = MIN_XPOS;
  if (MShape.yPos > MAX_YPOS) MShape.yPos = MAX_YPOS;
  if (MShape.yPos < MIN_YPOS) MShape.yPos = MIN_YPOS;

  if (!moved) {
    // Дошли до цели
    targetX = -1;
    targetY = -1;
    return;
  }

  MShape.drawFrame();
}
