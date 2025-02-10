#include "display.h"
#include "img.h"

Display matr(1); //set intensity=1

void scrollRightDispaly();
void digtalCounter();

void setup() {}

void loop() {
  scrollLeftString((char *)"abcdefghijklmnoprstuvwxqyz");
}

void scrollLeftString(char *str){
  unsigned long d = 100; int i = 0;
  do{
    matr.printChar(str[i]);
    delay(d * 2);
    for(int _i = 0; _i < 8; _i++){
      matr.scrollLeft(1);
      delay(d);
    }
    i++;
    delay(d * 4);
  }while(str[i] != '\0');
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


