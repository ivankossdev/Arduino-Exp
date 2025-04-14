#include "dsp_driver.h"

void clearEx() {
  for (int i = 0; i < 504; i++) {
    ex[i] = 0x00;
  }
}

void insertFig(unsigned char *arr, int lenArr, int posX, int posY) {
  unsigned char netPosition = 0, shift = 0;
  for (int i = posX; i < lenArr + posX; i++) {
    if (posY < 8) {
      ex[i + NEXT * 0] |= arr[i - posX] << posY;
      netPosition = arr[i - posX] >> (8 - posY);
      ex[i + NEXT * 1] |= netPosition;
      netPosition = 0;

    } else if (posY >= 8 && posY < 16) {
      shift = posY - 8;

      ex[i + NEXT * 1] |= arr[i - posX] << (shift);
      netPosition = arr[i - posX] >> (8 - shift);
      ex[i + NEXT * 2] |= netPosition;

      netPosition = 0;
      shift = 0;
    } else if (posY >= 16 && posY < 24) {
      shift = posY - 16;

      ex[i + NEXT * 2] |= arr[i - posX] << (shift);
      netPosition = arr[i - posX] >> (8 - shift);
      ex[i + NEXT * 3] |= netPosition;

      netPosition = 0;
      shift = 0;
    } else if (posY >= 24 && posY < 32) {
      shift = posY - 24;

      ex[i + NEXT * 3] |= arr[i - posX] << (shift);
      netPosition = arr[i - posX] >> (8 - shift);
      ex[i + NEXT * 4] |= netPosition;

      netPosition = 0;
      shift = 0;
    } else if (posY >= 32 && posY < 41) {
      shift = posY - 32;

      ex[i + NEXT * 4] |= arr[i - posX] << (shift);
      netPosition = arr[i - posX] >> (8 - shift);
      ex[i + NEXT * 5] |= netPosition;

      netPosition = 0;
      shift = 0;
    }
  }
}