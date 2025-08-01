#include "display.h"
void displayInit(Adafruit_SSD1306 *disp) {
  Serial.begin(115200);

  if (!disp->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  disp->clearDisplay();
  disp->display();
}

void progressBar(Adafruit_SSD1306 *disp) {
  disp->drawFastHLine(0, 0, SCREEN_WIDTH - 1, WHITE);
  disp->drawFastHLine(0, 15, SCREEN_WIDTH - 1, WHITE);
  disp->drawFastVLine(0, 0, 15, WHITE);
  disp->drawFastVLine(SCREEN_WIDTH - 1, 0, 15, WHITE);
  disp->display();

  disp->setCursor(0, 20);
  disp->setTextColor(SSD1306_WHITE);
  disp->setTextSize(2);

  for (int item = 2; item < 127; item++) {
    if ((item % 2) == 0) {
      disp->drawFastVLine(item, 2, 12, WHITE);
      disp->display();
    }

    disp->setCursor(0, 20);
    // disp->drawRect(80, 20, 40, 16, WHITE);
    disp->fillRect(80, 20, 40, 16, BLACK);
    disp->printf("Total: %d", item);
    disp->display();
  }
}

void clearProgressBar(Adafruit_SSD1306 *disp) {
  for (int item = 2; item < 127; item++) {
    disp->drawFastVLine(item, 2, 12, BLACK);
  }
  disp->setCursor(0, 20);
  disp->fillRect(80, 20, 40, 16, BLACK);
  disp->printf("Total: %d", 0);
  disp->display();
}

void countString(Adafruit_SSD1306 *disp) {
  disp->setTextColor(SSD1306_WHITE);
  disp->clearDisplay();
  disp->setTextSize(2);
  disp->display();
  delay(1000);

  disp->setTextSize(2);
  disp->setCursor(0, 0);
  for (int i = 0; i < 10; i++) {
    disp->write(i + '0');
    disp->display();
    delay(100);
  }
  delay(1000);
  disp->clearDisplay();
  disp->display();
  delay(1000);
}

void testScrolltext(Adafruit_SSD1306 *disp) {
  disp->clearDisplay();

  disp->setTextSize(2);  // Draw 2X-scale text
  disp->setTextColor(SSD1306_WHITE);
  disp->setCursor(0, 24);
  disp->println(F("hello led!"));
  disp->display();  // Show initial text
  delay(100);
  disp->startscrollright(0x00, 0x0F);
  delay(2000);
  disp->stopscroll();
  delay(1000);
  disp->startscrollleft(0x00, 0x0F);
  delay(2000);
  disp->stopscroll();
  delay(1000);
  disp->startscrolldiagright(0x00, 0x07);
  delay(2000);
  disp->startscrolldiagleft(0x00, 0x07);
  delay(2000);
  disp->stopscroll();
  delay(1000);
}

void showDot(Adafruit_SSD1306 *disp) {
  disp->setTextColor(SSD1306_WHITE);
  disp->clearDisplay();
  disp->setTextSize(2);
  disp->setCursor(0, 0);
  for (uint8_t i = 0; i < 10; i++) {
    disp->write('.');
    delay(100);
    disp->display();
  }
  delay(1000);
}