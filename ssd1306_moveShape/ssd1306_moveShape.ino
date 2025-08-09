#include "moveshape.h"

MoveShape moveshape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

void setup() { 
  moveshape.init();
}

void loop() {
  for(int i = 0; i < 127; i++){
    moveshape.shovPosition(i, i);
    delay(100);
  }
}
