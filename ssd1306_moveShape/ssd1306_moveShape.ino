#include "mShape.h"
#include "mv.h"

MShape MShape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MV mv;

void mvRight();
void mvDown();
void mvLeft();
void mvUp();
void downAndRight();
void upAndRigth();
void downAndLeft();
void upAndLeft();


void setup() {
  MShape.init();
  mv.init();
}

void loop() {
  downAndRight();
  upAndRigth();

  downAndRight();
  mvUp();

  downAndLeft();
  upAndLeft();

  downAndLeft();
  mvUp();

  // MShape.clearSh(on);
  // MShape.serDefaultXPos();
  // MShape.serDefaultYPos();
}

void mvRight() {
  while (MShape.xPos < 117) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.xPos;
  }
}

void mvDown() {
  while (MShape.yPos < 54) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.yPos;
  }
}

void mvLeft() {
  while (MShape.xPos > 0) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.xPos;
  }
}

void mvUp() {
  while (MShape.yPos > 16) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.yPos;
  }
}

void downAndRight() {
  while (MShape.yPos < 54) {
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.yPos; ++MShape.xPos;
  }
}

void upAndRigth(){
  while(MShape.yPos > 16){
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.yPos; ++MShape.xPos;
  }
}

void downAndLeft(){
  while(MShape.yPos < 54){
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.yPos; --MShape.xPos;
  }
}

void upAndLeft(){
  while(MShape.yPos > 16){
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.yPos; --MShape.xPos;
  }
}
