#include "mv.h"

void Logic_1();

void setup() {
  mshInit();
}

void loop() {
  for(int16_t i = MIN_YPOS ; i < 64; i++){
    mvCord(50, i);
  }
  clear();
}

void Logic_1() {
  downAndRight();
  upAndRigth();

  downAndRight();
  mvUp();

  downAndLeft();
  upAndLeft();

  downAndLeft();
  mvUp();
}
