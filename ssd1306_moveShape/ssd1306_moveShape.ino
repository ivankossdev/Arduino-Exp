#include "moveshape.h"

MoveShape moveshape(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

void setup() { 
  moveshape.init();
}

void loop() {
  int16_t xPos = 0, yPos = 16;
  do{
      moveshape.shovPosition(xPos, yPos);
      moveshape.movement(xPos, yPos);
      delay(10);
      xPos++; 

      if (xPos > 116){
        moveshape.clearSh(xPos, yPos);
      }

  } while (xPos < 117);
}
