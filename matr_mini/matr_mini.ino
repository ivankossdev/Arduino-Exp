#include "display.h"
#include "img.h"

Display matr(1); //set intensity=1

void scrollRightArrow();
void digtalCounter();

void setup() {}

void loop() {
  matr.insertShape((uint8_t *)arrowRight);
  matr.showDisplay();
  delay(80);
  for(int i = 0; i < 8; i++){
    matr.scrollRightLoop();
    delay(80);
  }
  matr.clearDisplay();
  delay(1000);
}

void scrollRightArrow(){
  matr.insertShape((uint8_t *)arrowRight);
  matr.showDisplay();
  delay(1000);

  for(uint8_t i = 0; i <= 9; i++){
    matr.scrollRight(1);
    delay(80);
  }

  matr.clearDisplay();
  delay(1000);
}

void digtalCounter(){
  for(int i = 0; i < 10; i++){
    matr.inserDigiatl(i | 0x30);
    matr.showDisplay();
    delay(500);
  }
}


