#include "MATR.h"

MATR matr(1); //set intensity=1

void setup() {

}

void loop() {
  for(uint8_t i = 0; i < 8; i++){
    matr.insertToDisplay(i, 0x0f);
    matr.showDisplay();
    delay(250);
  }
  matr.clearDisplay();
  matr.showDisplay();
  delay(250);
}
