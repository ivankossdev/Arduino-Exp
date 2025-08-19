#include "mv.h"

void setup() {
  mshInit();
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
}


