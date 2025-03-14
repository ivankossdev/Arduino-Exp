
#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <NOKIA5110_TEXT.h>
#include <cstring>

#define inverse  false
#define contrast 0xbf     // default is 0xBF set in LCDinit, Try 0xB1 <-> 0xBF, if your display is too dark/dim
#define bias 0x13         // LCD bias mode 1:48: Try 0x12 or 0x13 or 0x14 usually

/* Connection */
#define RST D4  // 4-RST
#define CE  D3  // 3-SCE
#define DC  D5  // 5-D/C
#define DIN D6  // 6-DN
#define CLK D7  // 7-SCLK

#define WIDTH_CHAR 7
#define OVERFLOW_DISPLAY 71

static NOKIA5110_TEXT mylcd(RST, CE, DC, DIN, CLK);

void displayInit();
void Count();
void NextRowString(bool delay_, char *str);

#endif