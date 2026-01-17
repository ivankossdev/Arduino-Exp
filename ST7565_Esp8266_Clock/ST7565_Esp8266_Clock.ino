#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif


U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/D8, /* dc=*/D4, /* reset=*/ 0);

void setup(void) {
  u8g2.begin();  
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
  } while ( u8g2.nextPage() );
  delay(500);
  m++;
  if ( m == 60 )
    m = 0;
}

