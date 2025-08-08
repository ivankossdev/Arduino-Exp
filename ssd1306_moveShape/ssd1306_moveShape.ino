#include "display.h"

Display display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

void setup() { 
  display.init();
}

void loop() {
  display.blinkRect(45, 30, 500);
  display.shapeCircle(20, 30);
}
