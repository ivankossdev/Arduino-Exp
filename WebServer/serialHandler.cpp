#include "serialHandler.h"


void SerialHandler::serialReader() {
  do {
    if (Serial.available() > 0) {
      serialData = Serial.readString();
      serialData.trim();
      isRead = false;
    }
  } while (isRead);
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

  isRead = true;
  serialReader();

  if (serialData.compareTo("on") == 0) {
    Serial.println("Relay ON");
    digitalWrite(14, LOW);
  } else if (serialData.compareTo("off") == 0) {
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

void SerialMenu::controllerName() {
  Serial.printf("Evidence Securiy Controller\n");
  Serial.printf("\n");
}

void SerialMenu::setIP() {
  Serial.printf("Network settings\n");

  if(confirmation("Continue? ")){ // 
    if (confirmation("Confirm your action")) {
      Serial.printf("Set action\n");

      IPAddress ip(192,168,0,60);     
      IPAddress gateway(192,168,0,1);   
      IPAddress subnet(255,255,255,0); 

      WiFi.config(ip, subnet, gateway);

      WiFi.reconnect();

      ipAddress();
    } else {
      Serial.printf("Cancel\n");
    }
  } else {
    Serial.printf("Сhanges are cancelled\n");
  }

  Serial.printf("\n");
}

bool SerialMenu::confirmation(String question) {
  bool state = false;
  isRead = true;

  Serial.printf(PSTR("%s \"yes\" or \"no\"\n"), question.c_str()); // 
  serialReader();

  if ((serialData.compareTo("yes") == 0) || (serialData.compareTo("y") == 0)) state = true;
  return state;
}

void SerialMenu::menu() {
  serialReader();

  if (serialData.compareTo("ip a") == 0) {
    Serial.println("Network info: ");
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
  } else if (serialData.compareTo("controller") == 0) {
    controllerName();
    serialData = "";
  } else if (serialData.compareTo("set ip") == 0) {
    setIP();
    serialData = "";
  }
}
