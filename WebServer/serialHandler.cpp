#include "serialHandler.h"


void SerialHandler::serialReader() {
  bool isRead = true;
  do{
    if (Serial.available() > 0) {
      serialData = Serial.readString();
      serialData.trim();
      isRead = false;
    }
  } while(isRead);
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
  Serial.printf("\n");
}

void SerialMenu::nameWiFi() {
  Serial.println("Network name: ");
  Serial.println(WiFi.SSID());
  Serial.printf("\n");
}

void SerialMenu::relayControl() {
  Serial.println("Relay cmd -> \"on\" | \"off\"");
  serialReader();

  if (serialData.compareTo("on") == 0){
    Serial.println("Relay ON");
    digitalWrite(14, LOW);
  } else if (serialData.compareTo("off") == 0){
    Serial.println("Relay OFF");
    digitalWrite(14, HIGH);
  }
  Serial.printf("\n");
}

void SerialMenu::help() {

  for (int i = 0; i < CMDCOUNT; i++) {
    Serial.printf("[%d] Terminal cmd -> \"%s\"\n", i, command[i]);
  }
  Serial.printf("\n");
}

void SerialMenu::menu() {
  serialReader();

  if (serialData.compareTo("ip a") == 0) {
    ipAddress();
    serialData = "";
  } else if (serialData.compareTo("wifi") == 0) {
    nameWiFi();
    serialData = "";
  } else if (serialData.compareTo("relay") == 0) {
    relayControl();
    serialData = "";
  } else if (serialData.compareTo("help") == 0) {
    help();
    serialData = "";
  }
}
