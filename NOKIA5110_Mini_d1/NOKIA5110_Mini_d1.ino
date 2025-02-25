#include <NOKIA5110_TEXT.h>

#define inverse  false
#define contrast 0xbf // default is 0xBF set in LCDinit, Try 0xB1 <-> 0xBF, if your display is too dark/dim
#define bias 0x13     // LCD bias mode 1:48: Try 0x12 or 0x13 or 0x14 usually

// LCD Nokia 5110 pinout left to right
// RST | CE | DC | DIN | CLK | VCC | LIGHT | GND 
// GPIO setup software SPI
#define RST D4  // 4-RST
#define CE  D3  // 3-SCE
#define DC  D5  // 5-D/C
#define DIN D6  // 6-DN
#define CLK D7  // 7-SCLK

// The constructor object, create an instance of the object
NOKIA5110_TEXT mylcd(RST, CE, DC, DIN, CLK);

void setup() {
  delay(50);
  mylcd.LCDInit(inverse, contrast, bias); // init the lCD
  mylcd.LCDClear(0x00); // clear whole screen
  mylcd.LCDFont(LCDFont_Default); // Set the font
}

void loop() {
  Test10();
}

//  TEST 10 Print function
void Test10(void)
{
  int c0 = 0, c1 = 0, c2 = 0;

  for(int i = 0; i <=999 ; i++){
    mylcd.LCDgotoXY(15, 1); 
    c2 = i / 100;
    c1 = i / 10 % 10;
    c0 = i % 10; 
    mylcd.printf("cnt-%d%d%d", c2, c1, c0);
    delay(500);
    // mylcd.LCDClear(0x00);
  }
}

