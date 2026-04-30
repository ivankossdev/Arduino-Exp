#include "UserInput.h"
#include "WiFiScanner.h"

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

bool UserInput::confirmWithTimeout(unsigned long timeoutMs) {
  Serial.print("Press 'y' to edit settings (");
  Serial.print(timeoutMs / 1000);
  Serial.println("s timeout): ");

  unsigned long startTime = millis();

  while (millis() - startTime < timeoutMs) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == 'y' || c == 'Y') {
        Serial.println("Editing mode activated");
        return true;
      }
      if (c == 'n' || c == 'N') {
        Serial.println("Starting with saved settings");
        return false;
      }
    }
    delay(10);
  }

  Serial.println("Timeout! Starting with saved settings...");
  return false;
}

String UserInput::readSSID(WiFiScanner& scanner) {
  // Сначала сканируем сети
  scanner.scanNetworks();

  Serial.println("\n--- WiFi Network Selection ---");
  Serial.println("Choose network:");
  Serial.println("1. Enter SSID manually");
  Serial.println("2. Select from list");

  String choice = readLine();

  if (choice == "2") {
    Serial.println("Enter network number: ");
    String numStr = readLine();
    int index = numStr.toInt() - 1;

    if (index >= 0 && index < scanner.getNetworkCount()) {
      String selectedSSID = scanner.getSSID(index);
      Serial.print("Selected: ");
      Serial.println(selectedSSID);
      return selectedSSID;
    } else {
      Serial.println("Invalid number, entering SSID manually:");
    }
  }

  // Ручной ввод
  Serial.println("Enter SSID: ");
  return readLine();
}

String UserInput::readPassword() {
  Serial.println("\n--- Password Setup ---");
  Serial.println("Enter WiFi password: ");
  return readLine();
}

String UserInput::readServerURL() {
  Serial.println("\n--- Server URL Setup ---");
  Serial.println("Enter server URL (e.g., http://192.168.1.100:8090/auth): ");
  //return readLine();
  return "http://192.168.0.175:8090/auth";
}

bool UserInput::confirmSelection() {
  Serial.println("\n--- Confirmation ---");
  Serial.println("Confirm these settings? (y/n): ");
  String response = readLine();
  return (response == "y" || response == "Y");
}
