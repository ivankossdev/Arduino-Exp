#include "display.h"

Display display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

void setup() { 
  display.Init();
}

void loop() {
  display.insRect(20, 30);
}
