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
    case '0': bufer = (uint8_t *)s_0; break;
    case '1': bufer = (uint8_t *)s_1; break;
    case '2': bufer = (uint8_t *)s_2; break;
    case '3': bufer = (uint8_t *)s_3; break;
    case '4': bufer = (uint8_t *)s_4; break;
    case '5': bufer = (uint8_t *)s_5; break;
    case '6': bufer = (uint8_t *)s_6; break;
    case '7': bufer = (uint8_t *)s_7; break;
    case '8': bufer = (uint8_t *)s_8; break;
    case '9': bufer = (uint8_t *)s_9; break;
    case 'a': bufer = (uint8_t *)s_a; break;
    case 'b': bufer = (uint8_t *)s_b; break;
    case 'c': bufer = (uint8_t *)s_c; break;
    case 'd': bufer = (uint8_t *)s_d; break;
    case 'e': bufer = (uint8_t *)s_e; break;
    case 'f': bufer = (uint8_t *)s_f; break;
    case 'g': bufer = (uint8_t *)s_g; break;
    case 'h': bufer = (uint8_t *)s_h; break;
    case 'i': bufer = (uint8_t *)s_i; break;
    case 'j': bufer = (uint8_t *)s_j; break;
    case 'k': bufer = (uint8_t *)s_k; break;
    case 'l': bufer = (uint8_t *)s_l; break;
    case 'm': bufer = (uint8_t *)s_m; break;
    case 'n': bufer = (uint8_t *)s_n; break;
    case 'o': bufer = (uint8_t *)s_o; break;
    case 'p': bufer = (uint8_t *)s_p; break;
    case 'r': bufer = (uint8_t *)s_r; break;
    case 's': bufer = (uint8_t *)s_s; break;
    case 't': bufer = (uint8_t *)s_t; break;
    case 'u': bufer = (uint8_t *)s_u; break;
    case 'v': bufer = (uint8_t *)s_v; break;
    case 'w': bufer = (uint8_t *)s_w; break;
    case 'q': bufer = (uint8_t *)s_q; break;
    case 'x': bufer = (uint8_t *)s_x; break;
    case 'y': bufer = (uint8_t *)s_y; break;
    case 'z': bufer = (uint8_t *)s_z; break;
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

void Display::printChar(char ch){
  insertToDispBuffer(getSimvolArray(ch));
  showDisplay();
}

void Display::scrollLeftString(char *str, unsigned long score){
  int i = 0;
  do{
    printChar(str[i]);
    Serial.printf("char[%d] %c width = %d\n", i, str[i], GetWidthShape((char *)getSimvolArray(str[i])));
    delay(score * 2);
    for(int _i = 0; _i < 8; _i++){
      scrollLeft(1);
      delay(score);
    }
    delay(score * 4);
    i++;
  }while(str[i] != '\0');
}

/* Рассчитывает ширину символа */
int Display::GetWidthShape(char * fig){
    int shift = 0;
    /* Расчет ширины символа */
    while(1){
        for(int i = 0; i < 8; i++){
            if((fig[i] << shift) & 0b10000000) return 8 - shift;
        }
        shift++;
        /* Значение по умолчанию */
        if (shift >= 8) return 3; 
    }
}








