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

struct YPos {
  bool str1;
  bool str2;
  bool str3;
  bool str4;
  bool str5;
  bool str6;
};

void printNextString(char *str) {
  int c = 0, posY = 0;
  YPos ypos;
  ypos.str1 = true;
  ypos.str2 = true;
  ypos.str3 = true;
  ypos.str4 = true;
  ypos.str5 = true;
  ypos.str6 = true;

  mylcd.LCDgotoXY(0, posY);

  do {
    if (str[c] == '\n') {
      if (c <= 12) {
        posY = 0;
        posY++;
      } else if (c > 12 && c <= 24) {
        posY = 1;
        posY++;
      } else if (c > 24 && c <= 48) {
        posY = 2;
        posY++;
      } else if (c > 48 && c <= 96) {
        posY = 3;
        posY++;
      }
      // mylcd.LCDClear(0x00);
      mylcd.LCDgotoXY(0, posY);
      c++;
    }

    mylcd.printf("%c", str[c]);
    delay(100);
    c++;
  } while (str[c] != '\0');
}
