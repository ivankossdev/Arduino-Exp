#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include "ds3231.h"
#include "serialSet.h"
 
U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/D8, /* dc=*/D4, /* reset=*/ 0);

// Переменные времени
int hours = 0;
int minutes = 0;
int seconds = 0;

bool colonVisible = true;
unsigned long lastUpdate = 0;

// Инициализация системных часов
SystemTime sysTime(DS3231);
SerialSet serialSet(115200);

void setup() {
  u8g2.begin();
  u8g2.setContrast(5);   // Оптимально для ST7565: 5 - 15
  u8g2.setFontMode(0);   // Режим с заливкой фона
  u8g2.setFontDirection(0);
  serialSet.Init();
}

void loop() {
  unsigned long currentMillis = millis();
  serialSet.SetCMD();
  sysTime.getTime();

  hours = sysTime.timeString[2]; 
  minutes = sysTime.timeString[1];
  seconds = sysTime.timeString[0]; 

  sysTime.getDate();
  sysTime.getMonth();
  sysTime.getYear();

  drawDigitalDisplay();

  // Мигающие точки 
  if (currentMillis - lastUpdate >= 500) {
    lastUpdate = currentMillis;
    colonVisible = !colonVisible;
  }
}

void drawDigitalDisplay() {
  u8g2.firstPage();
  do {
    // Очищаем дисплей
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 0, 128, 64);
    u8g2.setDrawColor(1);
    
    // 1. ОСНОВНОЕ ВРЕМЯ (14:30)
    drawMainTime();
    
    // 2. СЕКУНДЫ И ДАТА
    drawSecondaryInfo(sysTime.Year, sysTime.Month, sysTime.Date);
    
    // 3. ДЕКОРАТИВНЫЕ ЭЛЕМЕНТЫ
    drawDecorations();
    
  } while (u8g2.nextPage());
}

void drawMainTime() {
  char hourStr[3], minuteStr[3];
  
  // Форматирование с ведущим нулем
  sprintf(hourStr, "%02d", hours);
  sprintf(minuteStr, "%02d", minutes);
  
  u8g2.setFont(u8g2_font_logisoso20_tn);
  
  // Рассчитываем позиции
  int colonWidth = 10; // Ширина двоеточия
  int digitWidth = u8g2.getStrWidth("88"); // Ширина двух цифр
  int totalWidth = digitWidth * 2 + colonWidth;
  int startX = (128 - totalWidth) / 2;
  int yPos = 23; // Отступ сверху
  
  // Часы
  u8g2.drawStr(startX, yPos, hourStr);
  
  // Двоеточие (мигающее)
  if (colonVisible) {
    u8g2.setFont(u8g2_font_logisoso20_tn);
    u8g2.drawStr(startX + digitWidth, yPos, ":");
  }
  
  // Минуты
  u8g2.drawStr(startX + digitWidth + colonWidth, yPos, minuteStr);
}

void drawSecondaryInfo(int year, int month, int dateday) {
  char secondStr[3] = {'\0'};
  char date[16] = {'\0'};
  
  sprintf(secondStr, "%02d", seconds);
  sprintf(date, "20%d.%02d.%02d", year, month, dateday);
  
  // Секунды (под основным временем)
  u8g2.setFont(u8g2_font_logisoso16_tf); // 16px высотой
  int secWidth = u8g2.getStrWidth(secondStr);
  int secX = (128 - secWidth) / 2;
  u8g2.drawStr(secX, 40, secondStr);
  
  // Дата (пример)
  u8g2.setFont(u8g2_font_7x13B_tf); // 13px высотой
  u8g2.drawStr(30, 55, date);
}

void drawDecorations() {
  // Верхняя разделительная линия
  u8g2.drawHLine(10, 41, 108);
  
  // Угловые элементы
  u8g2.drawFrame(0, 0, 128, 64);
  u8g2.drawPixel(0, 0);
  u8g2.drawPixel(127, 0);
  u8g2.drawPixel(0, 63);
  u8g2.drawPixel(127, 63);
}

void drawWithProgressBar() {
  u8g2.firstPage();
  do {
    // Основное время
    drawMainTime();
    
    // Прогресс-бар секунд (0-59)
    int barWidth = map(seconds, 0, 59, 0, 118);
    int barY = 42;
    
    // Фон прогресс-бара
    u8g2.drawFrame(5, barY, 118, 6);
    // Заполнение
    u8g2.drawBox(5, barY, barWidth, 6);
    
    // Секунды цифрами над прогресс-баром
    u8g2.setFont(u8g2_font_logisoso16_tf);
    char secStr[3];
    sprintf(secStr, "%02d", seconds);
    int secWidth = u8g2.getStrWidth(secStr);
    u8g2.drawStr((128 - secWidth)/2, barY - 18, secStr);
    
    // Дата под прогресс-баром
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(40, barY + 15, "15.12.2024");
    
  } while(u8g2.nextPage());
}
