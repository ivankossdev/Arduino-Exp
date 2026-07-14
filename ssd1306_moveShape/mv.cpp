#include "mv.h"

int16_t targetX = -1;
int16_t targetY = -1;

unsigned long lastStopTime = 0;
bool isStopped = false;

bool isPulseGrowing = true;  // направление пульсации: растёт или уменьшается

MShape mShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void mshInit() {
  mShape.init();
}

// void mvCord(int16_t x, int16_t y) {
//   // 1. Устанавливаем координаты
//   mShape.xPos = x;
//   mShape.yPos = y;

//   // 2. Корректируем границы
//   if (mShape.xPos > MAX_XPOS) mShape.xPos = MAX_XPOS;
//   if (mShape.xPos < MIN_XPOS) mShape.xPos = MIN_XPOS;
//   if (mShape.yPos > MAX_YPOS) mShape.yPos = MAX_YPOS;
//   if (mShape.yPos < MIN_YPOS) mShape.yPos = MIN_YPOS;

//   // 3. Изменяем буфер (стираем старый квадрат, рисуем новый)
//   mShape.movement(mShape.xPos, mShape.yPos);  // внутри уже есть clearSh и fillRect

//   // 4. Выводим координаты (тоже в буфер)
//   mShape.dispCord();

//   // 5. Один раз отправляем буфер на экран
//   mShape.updateDisplay();
// }

// void mvRight() {
//   while (mShape.xPos < MAX_XPOS) {
//     mvCord(mShape.xPos + 1, mShape.yPos);
//   }
// }

void mvCord(int16_t x, int16_t y) {
  // Эту функцию мы теперь НЕ используем в цикле анимации.
  // Она нужна только для ручного управления кнопками (mvRight, mvLeft и т.д.)
  
  mShape.xPos = x;
  mShape.yPos = y;

  if (mShape.xPos > MAX_XPOS) mShape.xPos = MAX_XPOS;
  if (mShape.xPos < MIN_XPOS) mShape.xPos = MIN_XPOS;
  if (mShape.yPos > MAX_YPOS) mShape.yPos = MAX_YPOS;
  if (mShape.yPos < MIN_YPOS) mShape.yPos = MIN_YPOS;

  // Для ручного движения можно рисовать сразу, но аккуратно
  mShape.movement(mShape.xPos, mShape.yPos);
  mShape.dispCord();
  mShape.updateDisplay();
}

void mvDown() {
  while (mShape.yPos < MAX_YPOS) {
    mvCord(mShape.xPos, mShape.yPos + 1);
  }
}

void mvLeft() {
  while (mShape.xPos > MIN_XPOS) {
    mvCord(mShape.xPos - 1, mShape.yPos);
  }
}

void mvUp() {
  while (mShape.yPos > MIN_YPOS) {
    mvCord(mShape.xPos, mShape.yPos - 1);
  }
}

void downAndRight() {
  while (mShape.yPos < MAX_YPOS && mShape.xPos < MAX_XPOS) {
    mvCord(mShape.xPos + 1, mShape.yPos + 1);
  }
}

void upAndRight() {  // исправлено имя
  while (mShape.yPos > MIN_YPOS && mShape.xPos < MAX_XPOS) {
    mvCord(mShape.xPos + 1, mShape.yPos - 1);
  }
}

void downAndLeft() {
  while (mShape.yPos < MAX_YPOS && mShape.xPos > MIN_XPOS) {
    mvCord(mShape.xPos - 1, mShape.yPos + 1);
  }
}

void upAndLeft() {
  while (mShape.yPos > MIN_YPOS && mShape.xPos > MIN_XPOS) {
    mvCord(mShape.xPos - 1, mShape.yPos - 1);
  }
}

void clear() {
  mShape.clear();          // закрашиваем текущий квадрат чёрным
  mShape.updateDisplay();  // обновляем экран
}

void stepTowardsTarget() {
  if (targetX == -1 || targetY == -1) {
    return; 
  }

  bool movedX = false;
  bool movedY = false;

  // X
  if (mShape.xPos < targetX) {
    int dist = targetX - mShape.xPos;
    if (dist <= STEP_SIZE) {
      mShape.xPos = targetX;      // если осталось меньше шага — ставим ровно
      movedX = true;              // но считаем, что двигались
    } else {
      mShape.xPos += STEP_SIZE;  // иначе обычный шаг
      movedX = true;
    }
  } else if (mShape.xPos > targetX) {
    int dist = mShape.xPos - targetX;
    if (dist <= STEP_SIZE) {
      mShape.xPos = targetX;
      movedX = true;
    } else {
      mShape.xPos -= STEP_SIZE;
      movedX = true;
    }
  }
  // если xPos == targetX — movedX остаётся false

  // Y
  if (mShape.yPos < targetY) {
    int dist = targetY - mShape.yPos;
    if (dist <= STEP_SIZE) {
      mShape.yPos = targetY;
      movedY = true;
    } else {
      mShape.yPos += STEP_SIZE;
      movedY = true;
    }
  } else if (mShape.yPos > targetY) {
    int dist = mShape.yPos - targetY;
    if (dist <= STEP_SIZE) {
      mShape.yPos = targetY;
      movedY = true;
    } else {
      mShape.yPos -= STEP_SIZE;
      movedY = true;
    }
  }

  mShape.xPos = constrain(mShape.xPos, MIN_XPOS, MAX_XPOS);
  mShape.yPos = constrain(mShape.yPos, MIN_YPOS, MAX_YPOS);

  // Остановка только если по обеим осям мы уже не двигались
  if (!movedX && !movedY) {
    targetX = -1;
    targetY = -1;
  }
}





