#include "serialHandler.h"


void SerialHandler::serialReader() {
  if (Serial.available() > 0) {
    serialData = Serial.readString();
    serialData.trim();
  }
}

void SerialHandler::serialPrint() {

  int i = 0;
  do {
    if (serialData[i] == '\r' || serialData[i] == '\n') break;
    Serial.printf("%c", serialData[i]);
    i++;
  } while (serialData[i] != '\0');

  Serial.printf("\n");
}

void SerialMenu::ipAddress() {
  Serial.println("Network info: ");
  Serial.print("IP address  ");
  Serial.println(WiFi.localIP());

  Serial.print("Subnet mask ");
  Serial.println(WiFi.subnetMask());

  Serial.print("MAC         ");
  Serial.println(WiFi.macAddress());

  Serial.print("Gateway     ");
  Serial.println(WiFi.gatewayIP());
}

void SerialMenu::nameWiFi(){
  Serial.println("Network name: ");
  Serial.println(WiFi.SSID());
}

void SerialMenu::menu() {
  serialReader();

  if (serialData.compareTo("ip a") == 0) {
    ipAddress();
    serialData = "";
  } else if (serialData.compareTo("wifi") == 0) {
    nameWiFi();
    serialData = "";
  }
}
