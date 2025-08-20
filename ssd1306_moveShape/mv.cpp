#include "mv.h"

void mshInit() {
  MShape.init();
}

void mvCord(int16_t x, int16_t y) {
  MShape.xPos = x; MShape.yPos = y;

  if (MShape.xPos >= MAX_XPOS) {
    // MShape.clear();
    MShape.xPos = MAX_XPOS;
  }
  else if (MShape.xPos < MIN_XPOS) {
    // MShape.clear();
    MShape.xPos = MIN_XPOS;
  }
  else if (MShape.yPos >= MAX_YPOS) {
    // MShape.clear();
    MShape.yPos = MAX_YPOS;
  }
  else if (MShape.yPos < MIN_YPOS) {
    // MShape.clear();
    MShape.yPos = MIN_YPOS;
  }
  
  MShape.dispCord();
  MShape.movement(MShape.xPos, MShape.yPos);
}

void mvRight() {
  while (MShape.xPos < MAX_XPOS) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.xPos;
  }
}

void mvDown() {
  while (MShape.yPos < MAX_YPOS) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.yPos;
  }
}

void mvLeft() {
  while (MShape.xPos > MIN_XPOS) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.xPos;
  }
}

void mvUp() {
  while (MShape.yPos > MIN_YPOS) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.yPos;
  }
}

void downAndRight() {
  while (MShape.yPos < MAX_YPOS) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.yPos;
    ++MShape.xPos;
  }
}

void upAndRigth() {
  while (MShape.yPos > MIN_YPOS) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.yPos;
    ++MShape.xPos;
  }
}

void downAndLeft() {
  while (MShape.yPos < MAX_YPOS) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.yPos;
    --MShape.xPos;
  }
}

void upAndLeft() {
  while (MShape.yPos > MIN_YPOS) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.yPos;
    --MShape.xPos;
  }
}

void clear(){
  MShape.clear();
}