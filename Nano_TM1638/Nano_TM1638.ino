#include <TM1638.h>

//choose digital pins compatibles with your board
#define STB 2 // Strobe digital pin
#define CLK 3 // clock digital pin
#define DIO 4 // data digital pin

TM1638 tm(CLK,DIO,STB);
int8_t digit[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111}; 
unsigned int seg[4] = {'\0'};

unsigned int *decToSegment(int digital, int count){

    int x = 1; 

    do{
        count--;
        seg[count] = digital / x % 10; 
        x *= 10;
    } while( digital / x);

   return seg;  
}

void setup() {
  tm.reset();
}


void loop() {

  for(unsigned int i = 0; i < 999; i++){
    decToSegment(i, 4);
    tm.displayDig(2, digit[seg[3]]);
    tm.displayDig(3, digit[seg[2]]);
    tm.displayDig(4, digit[seg[1]]);
    tm.displayDig(5, digit[seg[0]]);

    delay(100);
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