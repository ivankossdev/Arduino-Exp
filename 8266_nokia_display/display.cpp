#include "display.h"

void displayInit(){
  delay(50);
  mylcd.LCDInit(inverse, contrast, bias); 
  mylcd.LCDClear(0x00); 
  mylcd.LCDFont(LCDFont_Default); 
}