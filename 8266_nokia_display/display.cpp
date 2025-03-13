#include "display.h"

void displayInit() {
  delay(50);
  mylcd.LCDInit(inverse, contrast, bias);
  mylcd.LCDClear(0x00);
  mylcd.LCDFont(LCDFont_Default);
}

void Count() {
  int c0 = 0, c1 = 0, c2 = 0;

  for (int i = 0; i <= 999; i++) {
    mylcd.LCDgotoXY(15, 1);
    c2 = i / 100;
    c1 = i / 10 % 10;
    c0 = i % 10;
    mylcd.printf("cnt-%d%d%d", c2, c1, c0);
    delay(250);
    // mylcd.LCDClear(0x00);
  }
}

void NextRowString(bool delay_, char *str) {
  int c = 0, posX = 0, posY = 0, lenstr = 0, overflow = 0;

  do {
    if (overflow > OVERFLOW_DISPLAY) {
      delay(2000);
      mylcd.LCDClear(0x00);
      posX = 0;
      posY = -1;
      overflow = 0;
    }
    if (lenstr < 12) {
      lenstr++;
    } else {
      posY++;
      posX = 0;
      lenstr = 1;
    }

    if (str[c] == '\n') {
      posY++;
      if (posY == 6 && overflow < OVERFLOW_DISPLAY) {
        delay(2000);
        mylcd.LCDClear(0x00);
        posY = 0;
        overflow = 0;
      }
      posX = 0;
      c++;
      lenstr = 1;
      if(str[c] == '\0') break;
    }

    mylcd.LCDgotoXY(posX++ * WIDTH_CHAR, posY);
    mylcd.LCDCharacter(str[c]);
    if(delay_) delay(100);
    overflow++;
    c++;

  } while (str[c] != '\0');
}
