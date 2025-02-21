#include "display.h"
#include "font.h"

uint8_t Display::reverseChar(uint8_t ch) {
  uint8_t rv = 0;
  for(int i = 0, t = 7; i < 8; i++, t--){
      rv |= ((ch >> i) & 0x01) << t;
  }
  
  return rv;
}

uint8_t * Display::getAsciiArray(uint8_t ch){
  
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
    case 'A': buffer = (uint8_t *)s_A; break;
    case 'B': buffer = (uint8_t *)s_B; break;
    case 'C': buffer = (uint8_t *)s_C; break;
    case 'D': buffer = (uint8_t *)s_D; break;
    case 'E': buffer = (uint8_t *)s_E; break;
    case 'F': buffer = (uint8_t *)s_F; break;
    case 'G': buffer = (uint8_t *)s_G; break;
    case 'H': buffer = (uint8_t *)s_H; break;
    case 'I': buffer = (uint8_t *)s_I; break;
    case 'J': buffer = (uint8_t *)s_J; break;
    case 'K': buffer = (uint8_t *)s_K; break;
    case 'L': buffer = (uint8_t *)s_L; break;
    case 'M': buffer = (uint8_t *)s_M; break;
    case 'N': buffer = (uint8_t *)s_N; break;
    case 'O': buffer = (uint8_t *)s_O; break;
    case 'P': buffer = (uint8_t *)s_P; break;
    case 'R': buffer = (uint8_t *)s_R; break;
    case 'S': buffer = (uint8_t *)s_S; break;
    case 'T': buffer = (uint8_t *)s_T; break;
    case 'U': buffer = (uint8_t *)s_U; break;
    case 'V': buffer = (uint8_t *)s_V; break;
    case 'W': buffer = (uint8_t *)s_W; break;
    case 'Q': buffer = (uint8_t *)s_Q; break;
    case 'X': buffer = (uint8_t *)s_X; break;
    case 'Y': buffer = (uint8_t *)s_Y; break;
    case 'Z': buffer = (uint8_t *)s_Z; break;
    case ' ': buffer = (uint8_t *)s_0x20; break;
    case '!': buffer = (uint8_t *)s_0x21; break;
    case '\"': buffer = (uint8_t *)s_0x22; break;
    case '#': buffer = (uint8_t *)s_0x23; break;
    case '$': buffer = (uint8_t *)s_0x24; break;
    case '%': buffer = (uint8_t *)s_0x25; break;
    case '&': buffer = (uint8_t *)s_0x26; break;
    case '\'': buffer = (uint8_t *)s_0x27; break;
    case '(': buffer = (uint8_t *)s_0x28; break;
    case ')': buffer = (uint8_t *)s_0x29; break;
    case '*': buffer = (uint8_t *)s_0x2a; break;
    case '+': buffer = (uint8_t *)s_0x2b; break;
    case ',': buffer = (uint8_t *)s_0x2c; break;
    case '-': buffer = (uint8_t *)s_0x2d; break;
    case '.': buffer = (uint8_t *)s_0x2e; break;
    case '/': buffer = (uint8_t *)s_0x2f; break;
    case ':': buffer = (uint8_t *)s_0x3a; break;
    case ';': buffer = (uint8_t *)s_0x3b; break;
    case '<': buffer = (uint8_t *)s_0x3c; break;
    case '=': buffer = (uint8_t *)s_0x3d; break;
    case '>': buffer = (uint8_t *)s_0x3e; break;
    case '?': buffer = (uint8_t *)s_0x3f; break;
    case '@': buffer = (uint8_t *)s_0x40; break;
    case '\\': buffer = (uint8_t *)s_0x5c; break;
    case '^': buffer = (uint8_t *)s_0x5e; break;
    case '|': buffer = (uint8_t *)s_0x7c; break;
  }
  return buffer;
}

uint8_t * Display::getCyrillicArray(uint8_t ch){
	switch(ch){
        case 0xd090: buffer = (uint8_t *)cyr_d090; break; // А
        case 0xd091: buffer = (uint8_t *)cyr_d091; break; // Б
        case 0xd092: buffer = (uint8_t *)cyr_d092; break; // В
        case 0xd093: buffer = (uint8_t *)cyr_d093; break; // Г
        case 0xd094: buffer = (uint8_t *)cyr_d094; break; // Д
        case 0xd095: buffer = (uint8_t *)cyr_d095; break; // Е
        case 0xd096: buffer = (uint8_t *)cyr_d096; break; // Ж
        case 0xd097: buffer = (uint8_t *)cyr_d097; break; // З
        case 0xd098: buffer = (uint8_t *)cyr_d098; break; // И
        case 0xd099: buffer = (uint8_t *)cyr_d099; break; // Й
        case 0xd09a: buffer = (uint8_t *)cyr_d09a; break; // К
        case 0xd09b: buffer = (uint8_t *)cyr_d09b; break; // Л
        case 0xd09c: buffer = (uint8_t *)cyr_d09c; break; // М
        case 0xd09d: buffer = (uint8_t *)cyr_d09d; break; // Н
        case 0xd09e: buffer = (uint8_t *)cyr_d09e; break; // О
        case 0xd09f: buffer = (uint8_t *)cyr_d09f; break; // П
        case 0xd0a0: buffer = (uint8_t *)cyr_d0a0; break; // Р
        case 0xd0a1: buffer = (uint8_t *)cyr_d0a1; break; // С
        case 0xd0a2: buffer = (uint8_t *)cyr_d0a2; break; // Т
        case 0xd0a3: buffer = (uint8_t *)cyr_d0a3; break; // У
        case 0xd0a4: buffer = (uint8_t *)cyr_d0a4; break; // Ф
        case 0xd0a5: buffer = (uint8_t *)cyr_d0a5; break; // Х
        case 0xd0a6: buffer = (uint8_t *)cyr_d0a6; break; // Ц
        case 0xd0a7: buffer = (uint8_t *)cyr_d0a7; break; // Ч
        case 0xd0a8: buffer = (uint8_t *)cyr_d0a8; break; // Ш
        case 0xd0a9: buffer = (uint8_t *)cyr_d0a9; break; // Щ
        case 0xd0aa: buffer = (uint8_t *)cyr_d0aa; break; // Ъ
        case 0xd0ab: buffer = (uint8_t *)cyr_d0ab; break; // Ы
        case 0xd0ac: buffer = (uint8_t *)cyr_d0ac; break; // Ь
        case 0xd0ad: buffer = (uint8_t *)cyr_d0ad; break; // Э
        case 0xd0ae: buffer = (uint8_t *)cyr_d0ae; break; // Ю
        case 0xd0af: buffer = (uint8_t *)cyr_d0af; break; // Я
        case 0xd0b0: buffer = (uint8_t *)cyr_d0b0; break; // а
        case 0xd0b1: buffer = (uint8_t *)cyr_d0b1; break; // б
        case 0xd0b2: buffer = (uint8_t *)cyr_d0b2; break; // в
        case 0xd0b3: buffer = (uint8_t *)cyr_d0b3; break; // г
        case 0xd0b4: buffer = (uint8_t *)cyr_d0b4; break; // д
        case 0xd0b5: buffer = (uint8_t *)cyr_d0b5; break; // е
        case 0xd0b6: buffer = (uint8_t *)cyr_d0b6; break; // ж
        case 0xd0b7: buffer = (uint8_t *)cyr_d0b7; break; // з
        case 0xd0b8: buffer = (uint8_t *)cyr_d0b8; break; // и
        case 0xd0b9: buffer = (uint8_t *)cyr_d0b9; break; // й
        case 0xd0ba: buffer = (uint8_t *)cyr_d0ba; break; // к
        case 0xd0bb: buffer = (uint8_t *)cyr_d0bb; break; // л
        case 0xd0bc: buffer = (uint8_t *)cyr_d0bc; break; // м
        case 0xd0bd: buffer = (uint8_t *)cyr_d0bd; break; // н
        case 0xd0be: buffer = (uint8_t *)cyr_d0be; break; // о
        case 0xd0bf: buffer = (uint8_t *)cyr_d0bf; break; // п
        case 0xd180: buffer = (uint8_t *)cyr_d180; break; // р
        case 0xd181: buffer = (uint8_t *)cyr_d181; break; // с
        case 0xd182: buffer = (uint8_t *)cyr_d182; break; // т
        case 0xd183: buffer = (uint8_t *)cyr_d183; break; // у
        case 0xd184: buffer = (uint8_t *)cyr_d184; break; // ф
        case 0xd185: buffer = (uint8_t *)cyr_d185; break; // х
        case 0xd186: buffer = (uint8_t *)cyr_d186; break; // ц
        case 0xd187: buffer = (uint8_t *)cyr_d187; break; // ч
        case 0xd188: buffer = (uint8_t *)cyr_d188; break; // ш
        case 0xd189: buffer = (uint8_t *)cyr_d189; break; // щ
        case 0xd18a: buffer = (uint8_t *)cyr_d18a; break; // ъ
        case 0xd18b: buffer = (uint8_t *)cyr_d18b; break; // ы
        case 0xd18c: buffer = (uint8_t *)cyr_d18c; break; // ь
        case 0xd18d: buffer = (uint8_t *)cyr_d18d; break; // э
        case 0xd18e: buffer = (uint8_t *)cyr_d18e; break; // ю
        case 0xd18f: buffer = (uint8_t *)cyr_d18f; break; // я

        default: * buffer = 0b10001011;
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
  insertToDispBuffer(getAsciiArray(dig));
}

void Display::inserDigiatl(uint8_t dig, int sh){
  insertToDispBuffer(getAsciiArray(dig), sh);
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
  insertToDispBuffer(getAsciiArray(ch));
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

/* Рассчитывает ширину символа */
int Display::getWidthShape(uint8_t  * fig){
    int shift = 0;
    /* Расчет ширины символа */
    while(1){
        for(int i = 0; i < 8; i++){
            if((fig[i] << shift) & 0b10000000) return 8 - shift;
        }
        /* Значение по умолчанию */
        if (shift >= 8) return 4; 
        shift++;
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

/* Функция бегущей строки */
void Display::scrollLeftString(unsigned long score){
  int i = 0; int w = 0;

  /* Читает строку по символьно с 0 индекса */
  do{
    /* Записывает новоу строку в eeprom по com порту */
    if(memory.writeString()){
      clearDisplay();
      break;
    }

    unsigned short int arrNum = 0;

    for(int _i = 7; _i > w; _i--){
      scrollLeft(1);
      /* Проверка символа memory.buffer[i] ACII или нет */
      if((memory.buffer[i] >> 8) & 1){
        arrNum |=  memory.buffer[i] << 8; i++; 
        arrNum |= memory.buffer[i] & 0xff;
        w = 6 - getWidthShape(getCyrillicArray(arrNum));
        sliceShapeByCordY(getCyrillicArray(arrNum), _i);
      }
      else{
        /* Сдвигает символ строки на 8 позиций */
        w = 6 - getWidthShape(getAsciiArray(memory.buffer[i]));
        sliceShapeByCordY(getAsciiArray(memory.buffer[i]), _i);
      }
      showDisplay();
      delay(score);
    }
    Serial.printf("%x %d %d\n", memory.buffer[i], i, arrNum);
    i++;
  }while(memory.buffer[i] != '\0');
}






