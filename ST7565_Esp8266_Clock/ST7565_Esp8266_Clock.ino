#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

/*
GMG12864-06D ver 2.2	ESP8266

                  CS	D8(CS) --- 15
                 RSE	3.3v
                  RS	D4 ------- 2
                 SCL	D5(SCLK) - 14
                  SI	D7(MOSI) - 13
                 VDD	3.3v
                 VSS	GND

A — Анод подсветки (+)	3.3v или PWM D
K — Катод подсветки (-)	GND
*/ 


U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/D8, /* dc=*/D4 );

void setup(void) {
  u8g2.begin();  
  u8g2.setContrast(15);
}

uint8_t m = 0;

void loop(void) {
  char m_str[3];
  
  strcpy(m_str, u8x8_u8toa(m, 2));		/* convert m to a string with two digits */
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_logisoso30_tr);
    u8g2.drawStr(0,31,"0");
    u8g2.drawStr(20,31,":");
    u8g2.drawStr(32,31,m_str);
    u8g2.drawHLine(0,33,127); 
    u8g2.drawVLine(70, 0, 33);
  } while ( u8g2.nextPage() );
  delay(1000);
  m++;
  if ( m == 60 )
    m = 0;
}

