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
  int c = 0, posX = 1, posY = 0;

  do {
    if (str[c] == '\n') {
      if (c <= 12) {
        posY++; 
        Serial.printf("string-1 %d Y=%d\n", c, posY);
      } else if (c > 12 && c <= 24) {
        posY++;
        Serial.printf("string-2 %d Y=%d\n", c - 12, posY);
      } else if (c > 24 && c <= 48) {
        posY++;
        Serial.printf("string-3 %d Y=%d\n", c, posY);
      } else if (c > 48 && c <= 96) {
        posY++;
        Serial.printf("string-4 %d Y=%d\n", c, posY);
      } else if (c > 96 && c <= 192) {
        posY++;
        Serial.printf("string-5 %d Y=%d\n", c, posY);
      }
      mylcd.LCDgotoXY(posX, posY);
      c++;
    }
    mylcd.LCDCharacter(str[c]);
    delay(100);
    c++;
  } while (str[c] != '\0');
  Serial.printf("\n");
}
