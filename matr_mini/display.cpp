#include "display.h"
#include "font.h"

uint8_t Display::reverseChar(uint8_t ch) {
  uint8_t rv = 0;
  for(int i = 0, t = 7; i < 8; i++, t--){
      rv |= ((ch >> i) & 0x01) << t;
  }
  
  return rv;
}

uint8_t * Display::getSimvolArray(uint8_t ch){
  uint8_t *bufer = (uint8_t *)calloc(8, sizeof(uint8_t));
  switch(ch){
    case 0: bufer = (uint8_t *)s_0; break;
    case 1: bufer = (uint8_t *)s_1; break;
    case 2: bufer = (uint8_t *)s_2; break;
    case 3: bufer = (uint8_t *)s_3; break;
    case 4: bufer = (uint8_t *)s_4; break;
    case 5: bufer = (uint8_t *)s_5; break;
    case 6: bufer = (uint8_t *)s_6; break;
    case 7: bufer = (uint8_t *)s_7; break;
    case 8: bufer = (uint8_t *)s_8; break;
    case 9: bufer = (uint8_t *)s_9; break;
  }
  return bufer;
}

void Display::insertToDispBuffer(uint8_t *array){
  for(int i = 0, r = 7; i < 8; i++, r--){
    displayBuffer[r] = reverseChar(array[i]);
  }
}

void Display::insertToDispBuffer(uint8_t *array, int sh){
  for(int i = 0, r = 7; i < 8; i++, r--){
    displayBuffer[r] = reverseChar(array[i] << sh);
  }
}

void Display::inserDigiatl(uint8_t dig){
  insertToDispBuffer(getSimvolArray(dig));
}

void Display::inserDigiatl(uint8_t dig, int sh){
  insertToDispBuffer(getSimvolArray(dig), sh);
}

void Display::insertShape(uint8_t *array){
  insertToDispBuffer(array);
}

void Display::insertShiftShape(uint8_t *array, int sh){
  if(sh > 8) sh = 0;
  insertToDispBuffer(array, sh);
}

void Display::scrollLeft(int scr){
  if(scr > 8) scr = 0;
  for(int i = 0; i < 8; i++){
    displayBuffer[i] = displayBuffer[i] >> scr;
  }
  showDisplay();
}

void Display::scrollRight(int scr){
  if(scr > 8) scr = 0;
  for(int i = 0; i < 8; i++){
    displayBuffer[i] = displayBuffer[i] << scr;
  }
  showDisplay();
}

void Display::scrollRightLoop(){
  uint8_t tmp = 0;

  for(int i = 7; i >= 0; i--){
    tmp = displayBuffer[i] >> 7;
    displayBuffer[i] = displayBuffer[i] << 1;
    displayBuffer[i] |= tmp;
  }

  showDisplay();
}









