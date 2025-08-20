#include "mv.h"

void mshInit(){
  MShape.init();
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
    ++MShape.yPos; ++MShape.xPos;
  }
}

void upAndRigth(){
  while(MShape.yPos > MIN_YPOS){
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.yPos; ++MShape.xPos;
  }
}

void downAndLeft(){
  while(MShape.yPos < MAX_YPOS){
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    ++MShape.yPos; --MShape.xPos;
  }
}

void upAndLeft(){
  while(MShape.yPos > MIN_YPOS){
    MShape.dispCord();
    MShape.movement(MShape.xPos, MShape.yPos);
    --MShape.yPos; --MShape.xPos;
  }
}