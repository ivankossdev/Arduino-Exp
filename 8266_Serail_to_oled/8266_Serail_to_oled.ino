#include "display.h"
#include "MyMillis.h"

MyDisplay dsp(&display);

MyMillis millis250(250);
MyMillis millis5000(5000);

String str;
bool readSerialPort();
bool messageState = true;
bool showMessage = true;
bool clearMessage = false;

void setup() {
  dsp.displayInit();
  dsp.displayPrintText((char*)"Init...");
  delay(1000);
  dsp.displayClear();
  dsp.displayPrintText((char*)"Test display\nPlese enter you \nmessage in terminal.");
  pinMode(D5, OUTPUT);
}

void loop() {
  if (showMessage) {
    delay(2000);
    Serial.println("\nEnter you message");
    showMessage = false;
  }

  if (readSerialPort() && messageState) {
    dsp.displayClear();
    dsp.displayPrintText((char*)"Serial message: ");
    dsp.displayPrintText(str);
    Serial.println(str);
    str = "";
    messageState = false;
    clearMessage = true; 
  }

  if (millis250.millisInterval()) {
    digitalWrite(D5, HIGH);
  } else {
    digitalWrite(D5, LOW);
  }

  if (clearMessage && millis5000.millisInterval()) {
    dsp.displayClear();
    dsp.displayPrintText((char*)"Test display\nPlese enter you \nmessage in terminal.");
    millis5000.reset();
    messageState = true; 
    clearMessage = false;
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