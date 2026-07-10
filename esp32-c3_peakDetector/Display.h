#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Инициализация дисплея (возвращает true при успехе)
bool initDisplay();

// Отрисовка индикатора уровня
// levelDB – текущий уровень (дБ), peakDB – значение пика (дБ)
void drawMeter(float levelDB, float peakDB);

#endif