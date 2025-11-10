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
  Serial.println(connectData.ssid);
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

void SerialMenu::writeNetworkData(String message, int *data) {
  do {
    Serial.println(message);
    isRead = true;
    serialReader();
    stringToIPaddress(serialData, data);
    for (int i = 0; i < 4; i++) {
      Serial.printf("[%d] ", data[i]);
    }
    Serial.printf("\n");
  } while(!(checkIpAddress(serialData)));
  Serial.printf("%s is write\n\n", message.c_str());
}

void SerialMenu::setIP() {
  Serial.printf("Network settings\n");

  if (confirmation("Continue? ")) {

    

    writeNetworkData("Set ip address", connectData.ip);
    writeNetworkData("Set subnet mask", connectData.subnet);
    writeNetworkData("Set gateway", connectData.gateway);

    IPAddress ip(connectData.ip[0], connectData.ip[1], connectData.ip[2], connectData.ip[3]);
    IPAddress subnet(connectData.subnet[0], connectData.subnet[1], connectData.subnet[2], connectData.subnet[3]);
    IPAddress gateway(connectData.gateway[0], connectData.gateway[1], connectData.gateway[2], connectData.gateway[3]);

    WiFi.config(ip, subnet, gateway);

    WiFi.reconnect();

    ipAddress();
    isSetIP = true;
  } else {
    Serial.printf("Сhanges are cancelled\n");
  }

  Serial.printf("\n");
}

bool SerialMenu::confirmation(String question) {
  bool state = false;
  isRead = true;

  Serial.printf(PSTR("%s \"yes\" or \"no\"\n"), question.c_str());  //
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
  } else if (serialData.compareTo("set ip") == 0) {
    setIP();
    serialData = "";
  } else if (serialData.compareTo("test") == 0) {
    
    Serial.println("Debug method checkIpAddress().");
    Serial.println("Enter ip address string:");
    
    bool status = false;
    do{
      isRead = true;
      serialReader();
      status = checkIpAddress(serialData); 
      if(status){
        Serial.println("String is a ip address");
      } else {
        Serial.println("String is a not ip address");
        Serial.println("Enter your IP address again:");
      }
    } while(!status);
    
    serialData = "";
    Serial.println("Debug is over");
    Serial.print("\n");
  }
}
