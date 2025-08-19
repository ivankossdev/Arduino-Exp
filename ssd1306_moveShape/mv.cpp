#include "mv.h"

void mshInit(){
  MShape.init();
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