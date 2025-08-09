#include "moveshape.h"

MoveShape moveshape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void mvRight() {
  while (moveshape.xPos < 117) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    delay(10);
    moveshape.xPos++;
  }
}

void mvDown() {
  while (moveshape.yPos < 54) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    delay(10);
    moveshape.yPos++;
  }
}

void mvLeft() {
  while (moveshape.xPos > 0) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    delay(10);
    moveshape.xPos--;
  }
}

void mvUp() {
  while (moveshape.yPos > 16) {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    delay(10);
    moveshape.yPos--;
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
