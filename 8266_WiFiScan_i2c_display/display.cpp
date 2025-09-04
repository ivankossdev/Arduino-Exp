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
  disp->setTextSize(1);
  disp->setTextColor(SSD1306_WHITE);
  disp->setCursor(0, 0);
  disp->clearDisplay();
}

void displayPrintText(Adafruit_SSD1306 *disp, char *data) {

  disp->println(data);
  disp->display();
}
void displayClear(Adafruit_SSD1306 *disp){
  disp->setCursor(0, 0);
  disp->clearDisplay();
  disp->display();
}

char found[512] = {'\0'};

void foundClear(){
  for(int i = 0; i < 512; i++) found[i] = '\0';
}
