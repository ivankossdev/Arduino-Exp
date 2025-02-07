#include "display.h"
#include "img.h"

Display matr(1); //set intensity=1

void setup() {}

void loop() {
  
  for(uint8_t i = 0; i <= 9; i++){
    matr.inserDigiatl(i, 1);
    matr.showDisplay();
    delay(250);
    matr.clearDisplay();
  }

  matr.insertShape((uint8_t *)arrowRight);

  matr.scrollLeft(1);
  delay(1000);

  matr.scrollRight(1);
  delay(1000);

  matr.clearDisplay();
}


