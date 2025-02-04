#include "MATR.h"

MATR matr(1); //set intensity=5

void setup() {

}

void loop() {
  matr.showDisplay();
  delay(500);
  matr.clearDisplay();
  matr.showDisplay();
  delay(500);
  matr.insertToDisplay(0x0f);
}
