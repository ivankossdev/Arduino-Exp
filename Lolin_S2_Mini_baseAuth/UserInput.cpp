// UserInput.cpp
#include "UserInput.h"

String UserInput::readLine() {
  String input = "";
  while (Serial.available() == 0); // Ждём ввода

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') break;
    input += c;
    delay(2);
  }
  return input;
}

String UserInput::readSSID() {
  Serial.println("Enter WiFi SSID (or number from list): ");
  return readLine();
}

String UserInput::readPassword() {
  Serial.println("Enter WiFi password: ");
  return readLine();
}

bool UserInput::confirmSelection() {
  Serial.println("Confirm selection? (y/n): ");
  String response = readLine();
  return (response == "y" || response == "Y");
}
