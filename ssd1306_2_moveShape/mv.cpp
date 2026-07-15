#include "mv.h"

int16_t targetX = -1;
int16_t targetY = -1;


void stepTowardsTarget(Shape& mShape) {
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
