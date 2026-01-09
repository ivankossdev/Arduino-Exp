#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

TM1637Display display(CLK, DIO);
int seconds = 0;
int minutes = 0;
bool colonState = true;
int endSecond = 59; 
int endMinutes = 59; 

void setup() {
  display.setBrightness(0x00);
}

void loop() {

  if (seconds > endSecond) {
    seconds = 0;
    minutes++;
  } 
  if (minutes > endMinutes){
    seconds = 0;
    minutes = 0;
  }
  int timeValue = minutes * 100 + seconds;
  
  display.showNumberDecEx(timeValue, colonState ? (0x80 >> 1) : 0b00000000, true);
  colonState = !colonState;

  seconds++;
  delay(1000);
}

