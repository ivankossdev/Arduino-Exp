#include "display.h"

MyDisplay dsp(&display); 
String str;
bool readSerialPort();

void setup() {
  dsp.displayInit();
  dsp.displayPrintText((char*)"Init...");
  delay(1000);
  dsp.displayClear();
  dsp.displayPrintText((char*)"Test display\nPlese enter you \nmessage in terminal.");
}

void loop() {
  if(readSerialPort()){
    dsp.displayClear();
    dsp.displayPrintText(str);
    str = "";
  }
}

bool readSerialPort() {
  bool status = false;
  if (Serial.available() > 0) {
    str = Serial.readString();
    status = true;
  }
  return status; 
}