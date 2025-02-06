#include "display.h"

Display matr(1); //set intensity=1

uint8_t fig1[8] = {
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
    matr.insertToDispBuffer(matr.GetSimvolArray(i));
    matr.showDisplay();
    delay(175);
    matr.clearDisplay();
  }

  matr.clearDisplay();
  matr.showDisplay();
  delay(1000);
  matr.insertToDispBuffer(fig1);
  matr.showDisplay();
  delay(1000);
  matr.clearDisplay();
  matr.showDisplay();
  delay(1000);
}


