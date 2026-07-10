#include "Display.h"
#include "constants.h"
#include <Wire.h>

// Глобальный объект дисплея (статический, чтобы был виден только в этом файле)
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool initDisplay() {
    Wire.begin(OLED_SDA, OLED_SCL);
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        return false;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    return true;
}

void drawMeter(float levelDB, float peakDB) {
    display.clearDisplay();

    // Рамка шкалы
    int barX = 5;
    int barY = 20;
    int barWidth = SCREEN_WIDTH - 10;
    int barHeight = 20;
    display.drawRect(barX - 1, barY - 1, barWidth + 2, barHeight + 2, SSD1306_WHITE);

    // Вычисляем заполнение (линейно от DB_MIN до DB_MAX)
    float percent = constrain((levelDB - DB_MIN) / (DB_MAX - DB_MIN), 0.0, 1.0);
    int fillWidth = (int)(percent * barWidth);
    if (fillWidth > 0) {
        display.fillRect(barX, barY, fillWidth, barHeight, SSD1306_WHITE);
    }

    // Маркер пика (вертикальная линия + метка сверху)
    float peakPercent = constrain((peakDB - DB_MIN) / (DB_MAX - DB_MIN), 0.0, 1.0);
    int peakX = barX + (int)(peakPercent * barWidth);
    display.drawLine(peakX, barY - 2, peakX, barY + barHeight + 2, SSD1306_WHITE);
    display.fillRect(peakX - 1, barY - 4, 3, 3, SSD1306_WHITE);

    // Подписи шкалы
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(barX, barY + barHeight + 4);
    display.print("-60");
    display.setCursor(barX + barWidth / 4 - 6, barY + barHeight + 4);
    display.print("-40");
    display.setCursor(barX + barWidth / 2 - 6, barY + barHeight + 4);
    display.print("-20");
    display.setCursor(barX + barWidth - 12, barY + barHeight + 4);
    display.print("0");

    // Цифровые значения
    display.setCursor(0, 0);
    display.print("Lvl: ");
    display.print(levelDB, 1);
    display.print(" dB");

    display.setCursor(0, 10);
    display.print("Peak: ");
    display.print(peakDB, 1);
    display.print(" dB");

    // Индикатор тишины
    if (levelDB <= DB_MIN + 0.5) {
        display.setCursor(80, 10);
        display.print("(mute)");
    }

    display.display();
}