#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;){}
  }
  display.clearDisplay();
}

void loop() {
  testdrawline();
}

void testdrawline() {
  int16_t i;

  display.clearDisplay();  // Clear display buffer

  // for (i = 0; i < display.width(); i += 4) {
  //   display.drawLine(0, 0, i, display.height() - 1, SSD1306_WHITE);
  //   display.display();  // Update screen with each newly-drawn line
  //   delay(1);
  // }

  // for (i = 0; i < display.height(); i += 4) {
  //   display.drawLine(0, 0, display.width() - 1, i, SSD1306_WHITE);
  //   display.display();
  //   delay(1);
  // }
  // delay(250);

  // display.clearDisplay();

  // for (i = 0; i < display.width(); i += 4) {
  //   display.drawLine(0, display.height() - 1, i, 0, SSD1306_WHITE);
  //   display.display();
  //   delay(1);
  // }
  // for (i = display.height() - 1; i >= 0; i -= 4) {
  //   display.drawLine(0, display.height() - 1, display.width() - 1, i, SSD1306_WHITE);
  //   display.display();
  //   delay(1);
  // }
  // delay(250);

  // display.clearDisplay();

  // for (i = display.width() - 1; i >= 0; i -= 4) {
  //   display.drawLine(display.width() - 1, display.height() - 1, i, 0, SSD1306_WHITE);
  //   display.display();
  //   delay(1);
  // }
  // for (i = display.height() - 1; i >= 0; i -= 4) {
  //   display.drawLine(display.width() - 1, display.height() - 1, 0, i, SSD1306_WHITE);
  //   display.display();
  //   delay(1);
  // }
  // delay(250);

  // display.clearDisplay();

  for (i = 0; i < display.height(); i += 1) {
    display.drawLine(display.width() - 1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  for (i = 0; i < display.width(); i += 1) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  for (i = 0; i < display.height(); i += 1) {
    display.drawLine(display.width() - 1, 0, 0, i, SSD1306_BLACK);
    display.display();
    delay(1);
  }

  for (i = 0; i < display.width(); i += 1) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, SSD1306_BLACK);
    display.display();
    delay(1);
  }

  // delay(2000);  // Pause for 2 seconds
}