#define GAIN      100
#define STEP      4
#define LOW_SOUND 50
#define MAX_SOUND 600

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
 
U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/D8, /* dc=*/D4, /* reset=*/ 0);

  int strela=-45;
  int hMeter = 64;                      
  int vMeter = 85;                      
  int rMeter = 83;
  int ur,urr,x;
 
void setup(void) {
  u8g2.begin(); 
  u8g2.setContrast(5); 
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_6x10_tf);
  // analogReference(INTERNAL);  // если очень маленький уровень сигнала
  pinMode(A0,INPUT);
}
 
void loop(void) {
  strela = map(log(2)*GAIN, LOW_SOUND,MAX_SOUND, -45,45);
  strela = constrain(strela, -45, 45);
  urr = strela;if(urr<ur){ur=ur-STEP;}else{ur = strela;}  

  u8g2.firstPage();
  do { 
  u8g2.drawLine(10, 17, 85, 17);// line 1
  u8g2.drawLine(88, 17, 125, 17);u8g2.drawLine(88, 16, 125, 16);// line 2
  u8g2.drawLine(3, 20, 85, 20);// line3  

  byte b1_2[10]{95,108,124,10,33,60,84,50,68,77};
  byte c1_2[10]{12,14,12,12,12,12,12,14,15,15};
  for(int i=0;i<10;i++){u8g2.drawLine(b1_2[i], 17, b1_2[i], c1_2[i]);} 
  byte b3[5]{3,25,48,67,84};
  for(int i=0;i<5;i++){u8g2.drawLine(b3[i], 20, b3[i], 24);}

  u8g2.setCursor(4, 10); u8g2.print(F("20  10   3   0 1"));
  u8g2.setCursor(2, 35); u8g2.print(F("0  25"));
  u8g2.setCursor(42, 35);  u8g2.print(F("50"));
  u8g2.setCursor(61, 35);u8g2.print(F("75"));
  u8g2.setCursor(76, 35);u8g2.print(F("100"));
  u8g2.setCursor(106, 10);u8g2.print(F("2"));
  u8g2.setCursor(122, 10);u8g2.print(F("3"));
  u8g2.setCursor(3, 55); u8g2.print(F("-"));
  u8g2.setCursor(119, 55);u8g2.print(F("+"));
  u8g2.setCursor(40, 55);u8g2.print(F("VU meter"));
  
  int a1 = (hMeter + (sin(ur / 57.296) * rMeter)); // meter needle horizontal coordinate
  int a2 = (vMeter - (cos(ur / 57.296) * rMeter)); // meter needle vertical coordinate
  u8g2.drawLine(a1, a2, hMeter, vMeter);
  u8g2.drawLine(a1-1, a2-1, hMeter-1, vMeter+1);
 
  } while ( u8g2.nextPage() );
  
}