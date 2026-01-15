#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "page.h"

const char *ssid = "ESP-AP";
const char *password = "12345678";

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", page.main());
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += server.argName(i) + ": " + server.arg(i) + "\n";
    }
    Serial.println(message);
    server.send(200, "text/plain", message);
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...\n");
  
  IPAddress ip(192,168,0,100);     
  IPAddress gateway(192,168,0,100);   
  IPAddress subnet(255,255,255,0);

  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(ssid, password, 5);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/postform/", handleForm);
   server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
