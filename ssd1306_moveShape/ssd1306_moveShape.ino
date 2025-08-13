#include "moveshape.h"

MoveShape moveshape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void mvRight();
void mvDown();
void mvLeft();
void mvUp();
void downAndRight();
void UpAndRigth();
void downAndLeft();


void setup() {
  moveshape.init();
}

void loop() {
  downAndRight();
  mvRight();
  mvUp();
  downAndLeft();
  mvLeft();
  UpAndRigth();
  mvLeft();
  // delay(1000);
  moveshape.clearSh();
  moveshape.serDefaultXPos();
  moveshape.serDefaultYPos();
}

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

void UpAndRigth(){
  while(moveshape.yPos > 16){
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    --moveshape.yPos; ++moveshape.xPos;
  }
}

void downAndLeft(){
  
  while(moveshape.yPos < 54){
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    ++moveshape.yPos; --moveshape.xPos;
  }
}
