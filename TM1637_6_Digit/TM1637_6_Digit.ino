#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

uint8_t data[6] = { '\0' };
TM1637Display display(CLK, DIO);
bool dotState = true;

int sec = 00;
int min = 47;
int hr = 14;

void setup() {
  display.setBrightness(0x0);
  // Serial.begin(9600);
}

void loop() {
  sec++;
  delay(1000);
  if (sec > 59) {
    sec = 0;
    min++;
  }

  if (min > 59) {
    min = 0;
    hr++;
  }

  if (hr > 23) {
    hr = 0;
  }
  
  printDig(showTime(hr, min, sec));
}

uint64_t showTime(int hr, int min, int sec) {
  uint64_t time = 0;
  time = hr * 100 + min;
  return time * 100 + sec;
}

void setZero(uint8_t mask) {
  for (int i = 0; i < 6; i++) {

    switch (i) {
      case 5: data[2] = (mask >> i) & 1 == 1 ? display.encodeDigit(0) : 0; break;
      case 4: data[1] = (mask >> i) & 1 == 1 ? display.encodeDigit(0) : 0; break;
      case 3: data[0] = (mask >> i) & 1 == 1 ? display.encodeDigit(0) : 0; break;
      case 2: data[5] = (mask >> i) & 1 == 1 ? display.encodeDigit(0) : 0; break;
      case 1: data[4] = (mask >> i) & 1 == 1 ? display.encodeDigit(0) : 0; break;
      case 0: data[3] = (mask >> i) & 1 == 1 ? display.encodeDigit(0) : 0; break;
    }
  }
}

void printDig(uint64_t digital) {
  int count = 6;
  uint64_t div = 1;
  uint64_t d = 0;

  setZero(0b00111111);

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

  /* Мигающие точки  */
  data[5] |= dotState ? 0b10000000 : (0b10000000 & ~0b10000000);
  data[1] |= dotState ? 0b10000000 : (0b10000000 & ~0b10000000);
  dotState = !dotState;

  display.setSegments(data, 6);
}