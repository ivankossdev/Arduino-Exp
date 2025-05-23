#include "dsp_driver.h"

void DspDriver::clearEx() {
  for (int i = 0; i < 504; i++) {
    ex[i] = 0x00;
  }
}

void DspDriver::insertFig(unsigned char *arr, int lenArr, int posX, int posY) {
  unsigned char nextPosition = 0, shift = 0;
  for (int i = posX; i < lenArr + posX; i++) {
    if (posY < 8) {
      ex[i + NEXT * 0] |= arr[i - posX] << posY;
      nextPosition = arr[i - posX] >> (8 - posY);
      ex[i + NEXT * 1] |= nextPosition;
      nextPosition = 0;

    } else if (posY >= 8 && posY < 16) {
      shift = posY - 8;

      ex[i + NEXT * 1] |= arr[i - posX] << (shift);
      nextPosition = arr[i - posX] >> (8 - shift);
      ex[i + NEXT * 2] |= nextPosition;

      nextPosition = 0;
      shift = 0;
    } else if (posY >= 16 && posY < 24) {
      shift = posY - 16;

      ex[i + NEXT * 2] |= arr[i - posX] << (shift);
      nextPosition = arr[i - posX] >> (8 - shift);
      ex[i + NEXT * 3] |= nextPosition;

      nextPosition = 0;
      shift = 0;
    } else if (posY >= 24 && posY < 32) {
      shift = posY - 24;

      ex[i + NEXT * 3] |= arr[i - posX] << (shift);
      nextPosition = arr[i - posX] >> (8 - shift);
      ex[i + NEXT * 4] |= nextPosition;

      nextPosition = 0;
      shift = 0;
    } else if (posY >= 32 && posY < 41) {
      shift = posY - 32;

      ex[i + NEXT * 4] |= arr[i - posX] << (shift);
      nextPosition = arr[i - posX] >> (8 - shift);
      ex[i + NEXT * 5] |= nextPosition;

      nextPosition = 0;
      shift = 0;
    }
  }
}

void DspDriver::shiftLeft(int row, int endCount) {

  for (int i = 1; i <= endCount; i++) {
    ex[i - 1 + NEXT * row] = ex[i + NEXT * row];
    ex[i + NEXT * row] = 0x00;
  }
}

void DspDriver::shifRigth(int row, int endCount) {
  unsigned char buffer = 0;
  int i_ = 0;

  if (endCount > 83) endCount = 83;

  for (int i = endCount; i > 0; i--) {
    i_ = i - 1;
    if (i_ >= 0) {
      ex[i + NEXT * row] = ex[i_ + NEXT * row];
      ex[i_ + NEXT * row] = 0x00;
    }
  }
}








