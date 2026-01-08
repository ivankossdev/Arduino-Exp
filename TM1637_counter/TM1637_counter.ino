#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

TM1637Display display(CLK, DIO);
uint8_t data[4] = {'\0'};

void setup() {
  display.setBrightness(0x00);
}

void loop() {
  for(int i = 0; i < 100; i++){
    display.showNumberDec(i, true);
    delay(100);
  }
}
