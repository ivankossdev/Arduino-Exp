#include "mv.h"

void Logic_1();

void setup() {
  mshInit();
}

void loop() {
  Logic_1();
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
