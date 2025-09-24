#include "display.h"

MyDisplay dsp(&display); 
String str;
void readString();

void setup() {
  dsp.displayInit();
  dsp.displayPrintText((char*)"Init...");
  delay(1000);
  dsp.displayClear();
  dsp.displayPrintText((char*)"Test display");
}

void loop() {
  readString();
}

void readString() {
  if (Serial.available() > 0) {
    str = Serial.readString();
    if (str.length() <= 512) {
      dsp.displayClear();
      dsp.displayPrintText(str);
    }
    str = "";
  }
}