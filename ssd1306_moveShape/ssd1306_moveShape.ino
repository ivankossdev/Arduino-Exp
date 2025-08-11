#include "moveshape.h"
#define SCORE 5

MoveShape moveshape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void mvRight() {
  while (moveshape.xPos < 117) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    ++moveshape.xPos;
    delay(SCORE);
  }
}

void mvDown() {
  while (moveshape.yPos < 54) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    ++moveshape.yPos;
    delay(SCORE);
  }
}

void mvLeft() {
  while (moveshape.xPos > 0) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    --moveshape.xPos;
    delay(SCORE);
  }
}

void mvUp() {
  while (moveshape.yPos > 16) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    --moveshape.yPos;
    delay(SCORE);
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
}
