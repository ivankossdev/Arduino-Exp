#include "display.h"

MyDisplay dsp(&display);
String str;
bool readSerialPort();
bool messageState = true;
bool showMessage = true;

unsigned long previousMillis = 0;
unsigned long currentMillis = 0; 
const long interval = 100;
int ledState = LOW;



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
  }

  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(D5, ledState);
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