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

void SerialMenu::getIpAddress() {
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

void SerialMenu::setWiFi() {
  Serial.println("WiFi Settings. ");

  Serial.println("Memory data: ");
  memory.clearBuffer();
  memory.readString(ENTERSSID);
  Serial.printf("Data 0 %s\n", memory.buffer);

  memory.clearBuffer();
  memory.readString(ENTERPASS);
  Serial.printf("Data 32 %s\n", memory.buffer);
  Serial.print("\n");
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
  } while (!(checkIpAddress(serialData)));
  Serial.printf("%s is write\n\n", message.c_str());
}

void SerialMenu::setIpAddress() {
  Serial.printf("Network settings\n");

  if (confirmation("Continue? ")) {

    writeNetworkData("Set ip address", connectData.ip);
    writeNetworkData("Set subnet mask", connectData.subnet);
    writeNetworkData("Set gateway", connectData.gateway);

    for (int i = 0; i < IPCNT; i++) {
      memory.writeIntData(connectData.ip[i], i + IPADDRESS);
    }

    for (int i = 0; i < IPCNT; i++) {
      memory.writeIntData(connectData.subnet[i], i + MASK);
    }

    for (int i = 0; i < IPCNT; i++) {
      memory.writeIntData(connectData.gateway[i], i + GETWAY);
    }

    IPAddress ip(connectData.ip[0], connectData.ip[1], connectData.ip[2], connectData.ip[3]);
    IPAddress subnet(connectData.subnet[0], connectData.subnet[1], connectData.subnet[2], connectData.subnet[3]);
    IPAddress gateway(connectData.gateway[0], connectData.gateway[1], connectData.gateway[2], connectData.gateway[3]);

    WiFi.config(ip, subnet, gateway);

    WiFi.reconnect();

    getIpAddress();
    isSetIP = true;
  } else {
    Serial.printf("Сhanges are cancelled\n");
  }

  Serial.printf("\n");
}

void SerialMenu::setIpAction() {
  Serial.println("Set ip action menu: ");
  Serial.print("Enter ip address ");
  writeNetworkData("action server: ", connectData.actionServer);
  for (int i = 0; i < IPCNT; i++) {
    memory.writeIntData(connectData.actionServer[i], i + IPACTION);
  }
  Serial.printf("\n");
}

void SerialMenu::getIpAction() {
  Serial.println("Address action server: ");
  int data[IPCNT] = { 0 };
  memory.readIntData(data, IPACTION, IPCNT);
  Serial.printf("IP: ");
  for (int i = 0; i < IPCNT; i++) {
    if (i < IPCNT - 1) {
      Serial.printf("%d.", data[i]);
    } else {
      Serial.printf("%d", data[i]);
    }
  }
  Serial.printf("\n\n");
}

bool SerialMenu::confirmation(String question) {
  bool state = false;
  isRead = true;

  Serial.printf(PSTR("%s \"yes\" or \"no\"\n"), question.c_str());  //
  serialReader();

  if ((serialData.compareTo("yes") == 0) || (serialData.compareTo("y") == 0)) state = true;
  return state;
}

void SerialMenu::getNetworkInMemory() {
  int data[ENDPOSITION] = { 0 };
  memory.readIntData(data, 0, ENDPOSITION);
  // Serial.printf("\n");
  // for (int i = IPACTION, i__ = 0; i < IPACTION + IPCNT; i++, i__++) {
  //   Serial.printf("[%d] = %d \n", i, data[i]);
  // }
  Serial.printf("\n");
  for (int i = IPADDRESS, i__ = 0; i < IPADDRESS + IPCNT; i++, i__++) {
    connectData.ip[i__] = data[i];
    Serial.printf("[%d] = %d \n", i, connectData.ip[i__]);
  }
  Serial.printf("\n");
  for (int i = MASK, i__ = 0; i < MASK + IPCNT; i++, i__++) {
    connectData.subnet[i__] = data[i]; 
    Serial.printf("[%d] = %d \n", i, connectData.subnet[i__]);
  }
  Serial.printf("\n");
  for (int i = GETWAY, i__ = 0; i < GETWAY + IPCNT; i++, i__++) {
    connectData.gateway[i__] = data[i];
    Serial.printf("[%d] = %d \n", i, connectData.gateway[i__]);
  }
}

void SerialMenu::menu() {
  serialReader();

  if (serialData.compareTo("get ip") == 0) {
    Serial.println("Network info: ");
    getIpAddress();
    serialData = "";
  } else if (serialData.compareTo("get wifi") == 0) {
    nameWiFi();
    serialData = "";
  } else if (serialData.compareTo("relay") == 0) {
    relayControl();
    serialData = "";
  } else if (serialData.compareTo("help") == 0) {
    help();
    serialData = "";
  } else if (serialData.compareTo("set ip") == 0) {
    setIpAddress();
    serialData = "";
  } else if (serialData.compareTo("set wifi") == 0) {
    setWiFi();
    serialData = "";
  } else if (serialData.compareTo("set action") == 0) {
    setIpAction();
    serialData = "";
  } else if (serialData.compareTo("get action") == 0) {
    getIpAction();
    serialData = "";
  } else if (serialData.compareTo("get memory") == 0) {
    getNetworkInMemory();
    serialData = "";
  }
}
