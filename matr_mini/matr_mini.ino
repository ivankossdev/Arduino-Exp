#include "display.h"
#include "img.h"

Display matr(1); //set intensity=1

void scrollRightArrow();

void setup() {}

void loop() {
  scrollRightArrow();
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


