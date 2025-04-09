#include "display.h"

unsigned char ex[504] = { '\0' };

void displayInit() {
  delay(50);
  mylcd.LCDInit(inverse, contrast, bias);
  mylcd.LCDClear(0x00);
}

void displayClear() {
  mylcd.LCDClear(0x00);
}

void clearEx() {
  for (int i = 0; i < 504; i++) {
    ex[i] = 0x00;
  }
  mylcd.LCDCustomChar(ex, sizeof(ex) / sizeof(unsigned char), LCDPadding_None, true);
}

void drawFullPixel(unsigned char pix){
  for (int i = 0; i < 84; i++) {
    ex[i + NEXT * 0] = pix;
    ex[i + NEXT * 1] = pix;
    ex[i + NEXT * 2] = pix;
    ex[i + NEXT * 3] = pix;
    ex[i + NEXT * 4] = pix;
    ex[i + NEXT * 5] = pix;
    mylcd.LCDCustomChar(ex, sizeof(ex) / sizeof(unsigned char), LCDPadding_None, true);
    delay(50);
  }
}

void insertFig(unsigned char *arr, int lenArr, int posX){
  for(int i = posX; i < lenArr + posX; i++){
    ex[i + NEXT * 0] = arr[i - posX];
  }
  mylcd.LCDCustomChar(ex, sizeof(ex) / sizeof(unsigned char), LCDPadding_None, true);
}










