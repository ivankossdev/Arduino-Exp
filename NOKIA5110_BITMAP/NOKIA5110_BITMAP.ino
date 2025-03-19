#include <NOKIA5110_TEXT.h>
#include "image.h"
#define inverse false
#define contrast 0xbf
#define bias 0x13

/* Connection */
#define RST D4  // 4-RST
#define CE D3   // 3-SCE
#define DC D5   // 5-D/C
#define DIN D6  // 6-DN
#define CLK D7  // 7-SCLK
static NOKIA5110_TEXT mylcd(RST, CE, DC, DIN, CLK);

void setup() {
  delay(50);
  mylcd.LCDInit(inverse, contrast, bias);
  mylcd.LCDClear(0x00);
}

void loop() {
  mylcd.LCDgotoXY(0, 0);
  mylcd.LCDCustomChar(ex, sizeof(ex) / sizeof(unsigned char), LCDPadding_None, true);
  mylcd.LCDgotoXY(6, 2);
  mylcd.printf("2025/03/17");

  int c0 = 0, c1 = 0;

  for (int i = 0; i < 60; i++) {
    c1 = i / 10 % 10;
    c0 = i % 10;
    mylcd.LCDgotoXY(9, 3);
    mylcd.printf("count %d%d", c1, c0);
    delay(250);
  }
  delay(1000);
}