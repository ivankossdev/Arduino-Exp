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
      Serial.printf("block else [ overflow=%d ], [ posY=%d ], [ lensrt=%d ], [ str[%d]=0x%x ]\n", overflow, posY, lenstr, c, str[c]);
      posX = 0;
      lenstr = 1;
    }

    if (str[c] == '\n') {
      Serial.printf("[ overflow=%d ], [ posY=%d ], [ lensrt=%d ], [ str[%d]=0x%x ]\n", overflow, posY, lenstr, c, str[c+1]);
      posY++;
      posX = 0;
      c++;
      if(lenstr < 12) posY--;
      lenstr = 0;
    }

    mylcd.LCDgotoXY(posX++ * WIDTH_CHAR, posY);
    mylcd.LCDCharacter(str[c]);
    delay(100);
    overflow++;
    c++;

  } while (str[c] != '\0');
  Serial.printf("\n\n");
}
