#include <NOKIA5110_TEXT.h>

#define inverse  false
#define contrast 0xbf     
#define bias 0x13         

/* Connection */
#define RST D4  // 4-RST
#define CE  D3  // 3-SCE
#define DC  D5  // 5-D/C
#define DIN D6  // 6-DN
#define CLK D7  // 7-SCLK
static NOKIA5110_TEXT mylcd(RST, CE, DC, DIN, CLK);

#define ARRAY 84

unsigned char mybitmap2[ARRAY] = {'\0'};

void setup() {
  delay(50);
  mylcd.LCDInit(inverse, contrast, bias); 
  mylcd.LCDClear(0x00); 
}

void loop() {
  for(int y = 0; y < 6; y++){
    for(int x = 0; x < ARRAY; x++){
      mylcd.LCDgotoXY(0, y);
      mybitmap2[x] = 0b11101111;
      mylcd.LCDCustomChar(mybitmap2, sizeof(mybitmap2) / sizeof(unsigned char), LCDPadding_None, true);
      delay(10);
    }
    for(int i = 0; i < ARRAY; i++){
      mybitmap2[i] = '\0';
    }
  }
  delay(5000);
  mylcd.LCDClear(0x00);
}
