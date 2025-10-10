#include "connectData.h"

void ConnectData::enterSerialDataInArray(char *data, String message) {
  Serial.println(message);

  while (Serial.available() > 0) {
    Serial.read();
  }

  while (true) {
    if (Serial.available() > 5) {
      serialData = Serial.readString();
      int i = 0;
      do {
        if (serialData[i] == '\r' || serialData[i] == '\n') break;
        data[i] = serialData[i];
        i++;
      } while (serialData[i] != '\0');
      data[i] = '\0';
      Serial.printf("%s - entered data\n", data);
      break;
    }

    delay(1000);
  }
}

void ConnectData::writeDataInArray(char *data, char *memData) {
  int i = 0;
  do {
    if (memData[i] == '\r' || memData[i] == '\n') break;
    data[i] = memData[i];
    i++;
  } while (memData[i] != '\0');
  data[i] = '\0';
}

void ConnectData::enterSSID() {
  enterSerialDataInArray(ssid, "Please enter WiFi network: ");
}

void ConnectData::enterSSID(char *memData_){
  writeDataInArray(ssid, memData_);
}

void ConnectData::enterPASS() {
  enterSerialDataInArray(pass, "Please enter password: ");
}

void ConnectData::enterPASS(char *memData_){
  writeDataInArray(pass, memData_);
}





