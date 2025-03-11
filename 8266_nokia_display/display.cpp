#include "display.h"

void displayInit() {
  delay(50);
  mylcd.LCDInit(inverse, contrast, bias);
  mylcd.LCDClear(0x00);
  mylcd.LCDFont(LCDFont_Default);
  Serial.begin(115200);
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

void NextRowString(char *str) {
  int c = 0, posX = 0, posY = 0, lenstr = 0;
  bool overflow = true;

  do {
    if((c > 72) && overflow){
      mylcd.LCDClear(0x00);
      posX = 0; posY = 0; overflow = false;

    }
    if (lenstr < 12) {
      lenstr++;
    } else {
      posY++;
      posX = 0;
      lenstr = 1;
      Serial.printf("\n");
    }

    if (str[c] == '\n') {
      posY++;
      posX = 0;
      c++;
      lenstr = 0;
    }

    Serial.printf("str[%d]=%c l=%d ", c, str[c], lenstr);
    mylcd.LCDgotoXY(posX++ * WIDTH_CHAR, posY);
    mylcd.LCDCharacter(str[c]);
    delay(100);

    c++;
    
  } while (str[c] != '\0');
  Serial.printf("\n\n\n");
}
