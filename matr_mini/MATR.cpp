#include "MATR.h"

MATR::MATR(uint8_t _intensity, byte dataPin, byte clockPin) {
  this->dataPin = dataPin;
  this->clockPin = clockPin;

  if (_intensity > 7) intensity = 7;
  else intensity = _intensity;

  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  Serial.begin(115200);
}


void MATR::sendCommand(byte cmd) {
  digitalWrite(dataPin, LOW);
  send(cmd);
  digitalWrite(dataPin, HIGH);
}


void MATR::sendData(byte address, byte data) {
  sendCommand(0x44);
  digitalWrite(dataPin, LOW);
  send(0xC0 | address);
  send(data);
  digitalWrite(dataPin, HIGH);
}


void MATR::send(byte data) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, data & 1 ? HIGH : LOW);
    data >>= 1;
    digitalWrite(clockPin, HIGH);
  }
}

void MATR::showDisplay() {
  for (uint8_t i = 0; i < 8; i++) {
    sendData(i, displayBuffer[i]);
    digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(dataPin, HIGH);
  }
  sendCommand(0x88 | (intensity));
}


void MATR::clearDisplay() {
  for (uint8_t i = 0; i < 8; i++) {
    displayBuffer[i] = 0x00;
  }
}
