#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
 
U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/D8, /* dc=*/D4, /* reset=*/ 0);

int h = 14, m = 30, s = 45; // 14:30:45

void setup() {
  u8g2.begin();
  u8g2.setContrast(5);
  u8g2.setDrawColor(1);
}

void loop() {
  drawInverseClock(h, m, s);
  
  // Имитация хода времени
  delay(10);
  s = (s+1)%60;
  if(s==0) m = (m+1)%60;z
  if(m==0 && s==0) h = (h+1)%24;
}

void drawInverseClock(int h, int m, int s) {
  u8g2.firstPage();
  do {
    // Инверсия фона
    u8g2.drawBox(0, 0, 128, 64);
    u8g2.setDrawColor(0); // Рисуем черным по белому
    
    // Белый круг циферблата
    u8g2.drawCircle(64, 32, 28);
    
    // Метки
    for(int i=0; i<12; i++) {
      float a = i*30*PI/180 - PI/2;
      u8g2.drawDisc(64+25*cos(a), 32+25*sin(a), 2);
    }
    
    // Стрелки
    float hAngle = (h%12 + m/60.0)*30*PI/180 - PI/2;
    float mAngle = (m + s/60.0)*6*PI/180 - PI/2;
    
    u8g2.drawTriangle(
      64, 32,
      64+12*cos(hAngle+0.2), 32+12*sin(hAngle+0.2),
      64+12*cos(hAngle-0.2), 32+12*sin(hAngle-0.2)
    );
    
    u8g2.drawTriangle(
      64, 32,
      64+20*cos(mAngle+0.15), 32+20*sin(mAngle+0.15),
      64+20*cos(mAngle-0.15), 32+20*sin(mAngle-0.15)
    );
    
    // Секундная стрелка
    u8g2.setDrawColor(1);
    float sAngle = s*6*PI/180 - PI/2;
    u8g2.drawLine(64, 32, 64+24*cos(sAngle), 32+24*sin(sAngle));
    
  } while(u8g2.nextPage());
}