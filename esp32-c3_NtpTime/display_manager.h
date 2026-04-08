#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

class DisplayManager {
private:
    Adafruit_SSD1306 display;

public:
    DisplayManager();
    void init();
    void showConnecting();
    void showConnected();
    void updateTime(int hour, int minute, int second, int day, int month, int year);
    void clear();

    // Новые методы‑обёртки
    void setTextSize(int size);
    void setCursor(int x, int y);
    void print(const String& text);
    void println(const String& text = "");
    void refreshDisplay(); 
};

#endif

