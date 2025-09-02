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

void displayPrintText(Adafruit_SSD1306 *disp) {
  disp->clearDisplay();
  disp->setTextSize(1);               
  disp->setTextColor(SSD1306_WHITE);  
  disp->setCursor(0, 0);              
  disp->println(F("ESP8266 WiFi scan example"));
  disp->display();
}