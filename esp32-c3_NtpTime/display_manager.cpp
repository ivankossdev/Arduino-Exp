#include "display_manager.h"
#include "time_utils.h"

DisplayManager::DisplayManager() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {}

void DisplayManager::init() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.display();
}

void DisplayManager::showConnecting() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Connecting to WiFi...");
    display.display();
}

void DisplayManager::showConnected() {
    display.clearDisplay();
    display.print("WiFi Connected!");
    display.display();
    delay(2000);
}

void DisplayManager::updateTime(int hour, int minute, int second, int day, int month, int year) {
    display.clearDisplay();

    // Отображение времени (ЧЧ:ММ:СС)
    display.setTextSize(2);
    display.setCursor(20, 25);
    if (hour < 10) display.print("0");
    display.print(hour);
    display.print(":");
    if (minute < 10) display.print("0");
    display.print(minute);
    display.print(":");
    if (second < 10) display.print("0");
    display.println(second);

    // Отображение даты (ДД/ММ/ГГГГ)
    String dayStr = TimeUtils::formatTwoDigits(day);
    String monthStr = TimeUtils::formatTwoDigits(month);
    String yearStr = String(year);

    display.setTextSize(1);
    display.setCursor(5, 50);
    display.print(dayStr + "/" + monthStr + "/" + yearStr);

    display.display();
}

void DisplayManager::clear() {
    display.clearDisplay();
    display.display();
}

void DisplayManager::setTextSize(int size) {
    display.setTextSize(size);
}

void DisplayManager::setCursor(int x, int y) {
    display.setCursor(x, y);
}

void DisplayManager::print(const String& text) {
    display.print(text);
}

void DisplayManager::println(const String& text) {
    if (text.length() > 0) {
        display.println(text);
    } else {
        display.println();
    }
}

void DisplayManager::refreshDisplay() {
    display.display();
}
