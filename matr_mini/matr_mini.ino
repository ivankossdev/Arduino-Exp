#include "display.h"
#include "img.h"

Display matr(1); //set intensity=1

void scrollRightArrow();
void digtalCounter();
void printSerialChars(char *str);

void setup() {}

void loop() {
  printSerialChars((char *)"abcdefghijklmnoprstuvwxqyz");
}

void scrollRightArrow(){
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

void printSerialChars(char *str){
  int i = 0;
  do{
    matr.printChar(str[i]);
    delay(500);
    i++;
  }while(str[i] != '\0');
}


