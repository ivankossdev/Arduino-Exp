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
  uint8_t *buffer = (uint8_t *)calloc(8, sizeof(uint8_t));
  switch(ch){
    case '0': buffer = (uint8_t *)s_0; break;
    case '1': buffer = (uint8_t *)s_1; break;
    case '2': buffer = (uint8_t *)s_2; break;
    case '3': buffer = (uint8_t *)s_3; break;
    case '4': buffer = (uint8_t *)s_4; break;
    case '5': buffer = (uint8_t *)s_5; break;
    case '6': buffer = (uint8_t *)s_6; break;
    case '7': buffer = (uint8_t *)s_7; break;
    case '8': buffer = (uint8_t *)s_8; break;
    case '9': buffer = (uint8_t *)s_9; break;
    case 'a': buffer = (uint8_t *)s_a; break;
    case 'b': buffer = (uint8_t *)s_b; break;
    case 'c': buffer = (uint8_t *)s_c; break;
    case 'd': buffer = (uint8_t *)s_d; break;
    case 'e': buffer = (uint8_t *)s_e; break;
    case 'f': buffer = (uint8_t *)s_f; break;
    case 'g': buffer = (uint8_t *)s_g; break;
    case 'h': buffer = (uint8_t *)s_h; break;
    case 'i': buffer = (uint8_t *)s_i; break;
    case 'j': buffer = (uint8_t *)s_j; break;
    case 'k': buffer = (uint8_t *)s_k; break;
    case 'l': buffer = (uint8_t *)s_l; break;
    case 'm': buffer = (uint8_t *)s_m; break;
    case 'n': buffer = (uint8_t *)s_n; break;
    case 'o': buffer = (uint8_t *)s_o; break;
    case 'p': buffer = (uint8_t *)s_p; break;
    case 'r': buffer = (uint8_t *)s_r; break;
    case 's': buffer = (uint8_t *)s_s; break;
    case 't': buffer = (uint8_t *)s_t; break;
    case 'u': buffer = (uint8_t *)s_u; break;
    case 'v': buffer = (uint8_t *)s_v; break;
    case 'w': buffer = (uint8_t *)s_w; break;
    case 'q': buffer = (uint8_t *)s_q; break;
    case 'x': buffer = (uint8_t *)s_x; break;
    case 'y': buffer = (uint8_t *)s_y; break;
    case 'z': buffer = (uint8_t *)s_z; break;
    case ' ': buffer = (uint8_t *)s_space; break;
  }
  return buffer;
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

char * Display::createString(char myInt){
    char buffer[2] = {'\0'}; 
    char * outData = (char *)calloc(9, sizeof(char));

    for(int item = 7; item >=0; item--){
        if(myInt >> item & 1) { sprintf(buffer, "@"); strncat(outData, buffer, 1);}
        else { sprintf(buffer, "."); strncat(outData, buffer, 1); };
    }
    return outData;
}

void Display::printSerialShape(uint8_t *fig){
    for(int i = 0; i < 8; i++)
        Serial.printf("%s\n", createString(fig[i]));
    Serial.printf("\n");
}

/* Рассчитывает ширину символа */
int Display::getWidthShape(uint8_t  * fig){
    int shift = 0;
    /* Расчет ширины символа */
    while(1){
        for(int i = 0; i < 8; i++){
            if((fig[i] << shift) & 0b10000000) return 8 - shift;
        }
        shift++;
        /* Значение по умолчанию */
        if (shift >= 8) return 4; 
    }
}

void Display::sliceShapeByCordY(uint8_t *shape, int position){
  if(position > 7) position = 7;
  int setZeroPozition = 8 - getWidthShape(shape);
  uint8_t shapePositoin = 0; 
  int serialCount = 0;
  for(int i = 0, r = 7; i < 8; i++, r--){
    shapePositoin = (uint8_t)(shape[i] << setZeroPozition);
    displayBuffer[r] |= reverseChar((shapePositoin & (0x01 << position)) >> position);
  }
}

/* Функция бегущей строки (в разработке) */
void Display::scrollLeftString(char *str, unsigned long score){
  int i = 0; int w = 0;

  /* Читает строку по символьно с 0 индекса */
  do{
    /* Сдвигает символ строки на 8 позиций */
    w = 6 - getWidthShape(getSimvolArray(str[i]));
    for(int _i = 7; _i > w; _i--){
      scrollLeft(1);
      sliceShapeByCordY(getSimvolArray(str[i]), _i);
      showDisplay();
      delay(score);
    }
    i++;
  }while(str[i] != '\0');
}






