#include "server.h"
#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

WiFiClient client;
HTTPClient http;

void showInfo() {
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Status OK.");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("IP: ");
  mylcd.LCDgotoXY(0, 2);
  mylcd.print(WiFi.localIP());
}

void actionOn() {
  server.send(200, "text/plain", "Evidence: \nAction ON");
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Evidence: ");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("Action ON");
  digitalWrite(D0, HIGH);
  delay(1000);
  showInfo();
  SendHTTP();
}

void actionOff() {
  server.send(200, "text/plain", "Evidence: \nAction OFF");
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Evidence: ");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("Action OFF");
  digitalWrite(D0, LOW);
  delay(1000);
  showInfo();
}

void motion() {
  server.send(200, "text/plain", "Evidence: \nCamera motion");
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Evidence: ");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("Camera 1");
  mylcd.LCDgotoXY(0, 2);
  mylcd.printf("Motion");
  delay(2000);
  showInfo();
}

void SendHTTP(){
      Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://jigsaw.w3.org/HTTP/connection.html")) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  }

