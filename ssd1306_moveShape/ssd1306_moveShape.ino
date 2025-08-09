#include "moveshape.h"

MoveShape moveshape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void mvRight(){

  do {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    delay(10);
    moveshape.xPos++;
  } while (moveshape.xPos < 117);
}

void mvDown(){
  do {
    moveshape.dispCord();
    moveshape.movement(moveshape.xPos, moveshape.yPos);
    delay(10);
    moveshape.yPos++;
  } while (moveshape.yPos <= 54);
}


void setup() {
  moveshape.init();
}

void loop() {
  mvRight();
  mvDown();
  delay(1000);
  moveshape.clearSh();
  delay(1000);
  moveshape.xPos = 0;
  moveshape.yPos = 16;
}


