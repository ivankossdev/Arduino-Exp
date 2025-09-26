#include "display.h"

MyDisplay dsp(&display);
String str;
bool readSerialPort();
bool messageState = true;
bool showMessage = true;

void setup() {
  dsp.displayInit();
  dsp.displayPrintText((char*)"Init...");
  delay(1000);
  dsp.displayClear();
  dsp.displayPrintText((char*)"Test display\nPlese enter you \nmessage in terminal.");
}

void loop() {
  if(showMessage){
    delay(2000);
    Serial.println("\nEnter you message");
    showMessage = false;
  }

  if (readSerialPort() && messageState) {
    dsp.displayClear();
    dsp.displayPrintText(str);
    Serial.println(str);
    str = "";
    messageState = false;
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