#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

uint8_t data[6] = { '\0' };

TM1637Display display(CLK, DIO);

void setup() {
  display.setBrightness(0x0);
}

void loop() {
  for (int i = 0; i < 60; i++) {
    prntSec(i);
    delay(250);
  }
}

void prntSec(unsigned int digital) {
  int count = 2;
  int x = 1;
  do {
    count--;
    int d = digital / x % 10;
    switch (count) {
      case 0: data[4] = display.encodeDigit(d); break;
      case 1: data[3] = display.encodeDigit(d); break;
    }
    x *= 10;
  } while (digital / x);

  data[2] = 0;
  data[1] = 0 | 0b10000000;
  data[0] = 0;
  data[5] = 0 | 0b10000000;

  display.setSegments(data, 6);
}