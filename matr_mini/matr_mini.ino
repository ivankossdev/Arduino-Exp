#include "MATR.h"
#include "font.h"

MATR matr(1); //set intensity=1

uint8_t fig1[8] = {
    0b00001000,
    0b00001100,
    0b01111110,
    0b01111111,
    0b01111110,
    0b00001100,
    0b00001000,
    0b00000000
};

void insertToDispArray(uint8_t *array);
uint8_t ReverseChar(uint8_t ch);
uint8_t *GetSimvolArray(uint8_t ch);

void setup() {}

void loop() {
  for(uint8_t i = 0; i <= 9; i++){
    insertToDispArray(GetSimvolArray(i));
    matr.showDisplay();
    delay(250);
    matr.clearDisplay();
  }
}

void insertToDispArray(uint8_t *array){
  for(int i = 0, r = 7; i < 8; i++, r--){
    matr.insertToDisplay(r, ReverseChar(array[i])); // 
  }
}

uint8_t ReverseChar(uint8_t ch){
    uint8_t rv = 0;
    for(int i = 0, t = 7; i < 8; i++, t--){
        rv |= ((ch >> i) & 0x01) << t;
    }
    
    return rv;
}

uint8_t *GetSimvolArray(uint8_t ch){
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

