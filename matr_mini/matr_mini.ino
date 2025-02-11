#include "display.h"
#include "img.h"

Display matr(1); //set intensity=1

void scrollRightDispaly();
void digtalCounter();

char example[32] = "abc";

void setup() {}

void loop() {
  matr.scrollLeftString(example, 100);
}

void scrollRightDispaly(){
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

void digtalCounter(){
  for(int i = 0; i < 10; i++){
    matr.inserDigiatl(i | 0x30);
    matr.showDisplay();
    delay(500);
  }
}


