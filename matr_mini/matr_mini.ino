#include "display.h"

Display matr(1); //set intensity=1

uint8_t arrowRight[8] = {
    0b00001000,
    0b00001100,
    0b01111110,
    0b01111111,
    0b01111110,
    0b00001100,
    0b00001000,
    0b00000000
};

void setup() {}

void loop() {
  
  for(uint8_t i = 0; i <= 9; i++){
    matr.inserDigiatl(i);
    matr.showDisplay();
    delay(1000);
    matr.clearDisplay();
  }

  matr.clearDisplay();
  matr.showDisplay();
  delay(1000);

  matr.clearDisplay();
  matr.insertShape(arrowRight);
  matr.showDisplay();
  delay(1000);

  matr.clearDisplay();
  matr.insertShiftShape(arrowRight, 1); 
  matr.showDisplay();
  delay(1000);

  matr.clearDisplay();
  matr.showDisplay();
  delay(1000);

}


