#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

#define TEST_DELAY   2000
uint8_t data[6] = {'\0'};


TM1637Display display(CLK, DIO);

void setup()
{
  display.setBrightness(0x0);
}

void loop()
{
  data[0] = display.encodeDigit(2);
  data[1] = display.encodeDigit(1);
  data[2] = display.encodeDigit(0);
  data[3] = display.encodeDigit(5);
  data[4] = display.encodeDigit(4);
  data[5] = display.encodeDigit(3);
  display.setSegments(data, 6);
}
