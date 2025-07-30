
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.display();
}

void loop() {
  // testScrolltext(&display);
  showDot(&display);
  // countString(&display);
  for (uint16_t y = 0; y < 15; y++) {
    display.drawFastHLine(0, y, 128, WHITE);
    display.display();
  }
  delay(1000);

  for (uint16_t x = 0; x < 128; x++) {
    display.drawFastVLine(x, 0, 128, BLACK);
    display.display();
  }
  delay(1000);
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

  // Scroll in various directions, pausing in-between:
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
