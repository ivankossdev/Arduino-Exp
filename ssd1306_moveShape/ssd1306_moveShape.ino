#include "moveshape.h"

MoveShape moveshape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void mvRight() {
  while (moveshape.xPos < 117) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    ++moveshape.xPos;
  }
}

void mvDown() {
  while (moveshape.yPos < 54) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    ++moveshape.yPos;
  }
}

void mvLeft() {
  while (moveshape.xPos > 0) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    --moveshape.xPos;
  }
}

void mvUp() {
  while (moveshape.yPos > 16) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    --moveshape.yPos;
  }
}

void downAndRight() {
  while (moveshape.yPos < 54) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    ++moveshape.yPos; ++moveshape.xPos;
  }
}


void setup() {
  moveshape.init();
}

void loop() {
  mvRight();
  mvDown();
  mvLeft();
  mvUp();
  downAndRight();
  mvRight();
  moveshape.clearSh();
  delay(1000);
  moveshape.serDefaultXPos();
  moveshape.serDefaultYPos();
}
