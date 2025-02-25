#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NOKIA5110_TEXT.h>

#define inverse  false
#define contrast 0xbf 
#define bias 0x13     
#define RST D4  // 4-RST
#define CE  D3  // 3-SCE
#define DC  D5  // 5-D/C
#define DIN D6  // 6-DN
#define CLK D7  // 7-SCLK

NOKIA5110_TEXT mylcd(RST, CE, DC, DIN, CLK);
#ifndef STASSID
#define STASSID "HUAWEI-V4XQZZ_HiLink"
#define STAPSK "12345678"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

void handleRoot() {
  
  server.send(200, "text/plain", "Status OK.\r\n");
  mylcd.LCDClear(0x00);
  mylcd.LCDgotoXY(0, 0);
  mylcd.printf("Status OK.");
  mylcd.LCDgotoXY(0, 1);
  mylcd.printf("IP: ");
  mylcd.LCDgotoXY(0, 2);
  mylcd.print(WiFi.localIP());
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

void setup(void) {
  /* INIT LCD */
  delay(50);
  mylcd.LCDInit(inverse, contrast, bias); // init the lCD
  mylcd.LCDClear(0x00); // clear whole screen
  mylcd.LCDFont(LCDFont_Default);
  /* INIT Serial */
  Serial.begin(115200);
  /* INIT WIFI */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());



  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", handleRoot);

  server.on("/action_on", []() {
    server.send(200, "text/plain", "Evidence: \nAction ON");
    mylcd.LCDClear(0x00);
    mylcd.LCDgotoXY(0, 0);
    mylcd.printf("Evidence: ");
    mylcd.LCDgotoXY(0, 1);
    mylcd.printf("Action ON");
  });

  server.on("/action_off", []() {
    server.send(200, "text/plain", "Evidence: \nAction OFF");
    mylcd.LCDClear(0x00);
    mylcd.LCDgotoXY(0, 0);
    mylcd.printf("Evidence: ");
    mylcd.LCDgotoXY(0, 1);
    mylcd.printf("Action OFF");
  });

  server.onNotFound(handleNotFound);

  /////////////////////////////////////////////////////////
  // Hook examples

  server.addHook([](const String& method, const String& url, WiFiClient* client, ESP8266WebServer::ContentTypeFunction contentType) {
    (void)method;       // GET, PUT, ...
    (void)url;          // example: /root/myfile.html
    (void)client;       // the webserver tcp client connection
    (void)contentType;  // contentType(".html") => "text/html"
    Serial.printf("A useless web hook has passed\n");
    Serial.printf("(this hook is in 0x%08x area (401x=IRAM 402x=FLASH))\n", esp_get_program_counter());
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  server.addHook([](const String&, const String& url, WiFiClient*, ESP8266WebServer::ContentTypeFunction) {
    if (url.startsWith("/fail")) {
      Serial.printf("An always failing web hook has been triggered\n");
      return ESP8266WebServer::CLIENT_MUST_STOP;
    }
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  server.addHook([](const String&, const String& url, WiFiClient* client, ESP8266WebServer::ContentTypeFunction) {
    if (url.startsWith("/dump")) {
      Serial.printf("The dumper web hook is on the run\n");

      // Here the request is not interpreted, so we cannot for sure
      // swallow the exact amount matching the full request+content,
      // hence the tcp connection cannot be handled anymore by the
      // webserver.
#ifdef STREAMSEND_API
      // we are lucky
      client->sendAll(Serial, 500);
#else
      auto last = millis();
      while ((millis() - last) < 500) {
        char buf[32];
        size_t len = client->read((uint8_t*)buf, sizeof(buf));
        if (len > 0) {
          Serial.printf("(<%d> chars)", (int)len);
          Serial.write(buf, len);
          last = millis();
        }
      }
#endif
      // Two choices: return MUST STOP and webserver will close it
      //                       (we already have the example with '/fail' hook)
      // or                  IS GIVEN and webserver will forget it
      // trying with IS GIVEN and storing it on a dumb WiFiClient.
      // check the client connection: it should not immediately be closed
      // (make another '/dump' one to close the first)
      Serial.printf("\nTelling server to forget this connection\n");
      static WiFiClient forgetme = *client;  // stop previous one if present and transfer client refcounter
      return ESP8266WebServer::CLIENT_IS_GIVEN;
    }
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  // Hook examples
  /////////////////////////////////////////////////////////

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
