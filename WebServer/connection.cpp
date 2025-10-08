#include "connection.h"
void Connection::enterSerialDataInArray(char *data, String message) {
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
