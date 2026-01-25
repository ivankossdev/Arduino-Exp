#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

uint8_t data[6] = { '\0' };
TM1637Display display(CLK, DIO);
bool dotState = true;

void setup() {
  display.setBrightness(0x2);
  // Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < 60; i++) {
    printDig(i);
    delay(250);
  }
}

void printDig(uint64_t digital) {
  int count = 6;
  uint64_t div = 1;
  uint64_t d = 0;

  for (int i = 0; i < count; i++) data[i] = 0;
  data[4] = display.encodeDigit(0);
  /* Мигающие точки  */
  data[5] |= dotState ? 0b10000000 : (0b10000000 & ~0b10000000);
  data[1] |= dotState ? 0b10000000 : (0b10000000 & ~0b10000000);
  dotState = !dotState;

  do {
    count--;
    d = digital / div % 10;

    switch (count) {
      case 0: data[2] = display.encodeDigit(d); break;
      case 1: data[1] = display.encodeDigit(d); break;
      case 2: data[0] = display.encodeDigit(d); break;
      case 3: data[5] = display.encodeDigit(d); break;
      case 4: data[4] = display.encodeDigit(d); break;
      case 5: data[3] = display.encodeDigit(d); break;
    }

    div *= 10;
  } while (digital / div);

  display.setSegments(data, 6);
}