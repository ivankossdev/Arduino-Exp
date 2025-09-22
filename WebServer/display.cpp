#include "display.h"

MyDisplay::MyDisplay(Adafruit_SSD1306 *disp){
  this->d = disp; 
}

void MyDisplay::displayInit() {
  Serial.begin(115200);

  if (!d->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  d->clearDisplay();
  d->display();
  d->setTextSize(1);
  d->setTextColor(SSD1306_WHITE);
  d->setCursor(0, 0);
  d->clearDisplay();
}

void MyDisplay::displayPrintText(char *data) {
  d->println(data);
  d->display();
}
void MyDisplay::displayClear(){
  d->setCursor(0, 0);
  d->clearDisplay();
  d->display();
}

void MyDisplay::foundClear(){
  for(int i = 0; i < 512; i++) this->found[i] = '\0';
}
