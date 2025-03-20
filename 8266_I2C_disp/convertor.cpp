#include "convertor.h"

int Convertor::FromEightToDec(int c){
  return ((c >> 4) * 10 + (0x0F & c));
}

int Convertor::FromDecToEight(int c){
  return ((c / 10) << 4) | (c % 10);
}