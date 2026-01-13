#include <TM1638.h>

//choose digital pins compatibles with your board
#define STB 2 // Strobe digital pin
#define CLK 3 // clock digital pin
#define DIO 4 // data digital pin

TM1638 tm(CLK,DIO,STB);
int8_t digit[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111}; 

void setup() {
  tm.reset();
}


void loop() {

  for(int i = 0, i_ = 9; i < 10; i++, i_--){
    tm.displayDig(0, digit[i]);
    tm.displayDig(7, digit[i_]);
    delay(500);
  }

  uint8_t buttons = tm.getButtons();
  tm.writeLeds(buttons);
  
}

  // static uint32_t timer = millis();
  
  // if ( millis()-timer > 1000){
  //   timer=millis();
  //   tm.displaySetBrightness(pulse);
  //   pulse = (pulse==PULSE1_16) ? PULSE14_16 : PULSE1_16;
  // }