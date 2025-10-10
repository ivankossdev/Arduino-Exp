#include <ESP8266WiFi.h>
#include "scanWiFi.h"
#include "display.h"
#include "MemHandler.h"
#include "connectData.h"

//HUAWEI-V4XQZZ_HiLink

WiFiServer server(80);
Memory memory;
String header;
String portD5State = "off";

const int pin14 = 14;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;
bool stateConnection = false;
void clientHandler();
void printConnectedInfo();

void setup() {
  Serial.begin(115200);
  dsp.displayInit();

  /* Сканирование доступный сетей */
  dsp.displayPrintText((char *)"Scan WiFi networks");
  searchWiFi();

  /* Авторизайия в сети WiFi */
  dsp.displayClear();

  dsp.displayPrintText((char *)"Please enter WiFi network: ");
  connectData.enterSSID();
  dsp.displayClear();
  dsp.displayPrintText((char *)"Please enter password: ");
  connectData.enterPASS();

  /* Инициализация портов */
  pinMode(pin14, OUTPUT);
  digitalWrite(pin14, LOW);

  dsp.displayClear();
  dsp.displayPrintText((char *)"Connetcting to WiFi...");
  Serial.println("Connetcting to \nWiFi...");

  /* Подключение к сети WiFi */
  Serial.print("Connecting to ");
  Serial.println(connectData.ssid);
  WiFi.begin(connectData.ssid, connectData.pass);

  int timeOutCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    timeOutCount++;
    if (timeOutCount > 20) {
      stateConnection = true;
      break;
    }
  }
  if (stateConnection) {
    Serial.println("");
    Serial.println("Error WiFi connected.");

    dsp.displayClear();
    dsp.displayPrintText((char *)"Error WiFi connected.");

  } else {
    /* Данные в консоли */
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();

    /* Данные на дисплее */
    printConnectedInfo();
  }
}

void loop() {
  if (!stateConnection) {
    clientHandler();
  }
}

void clientHandler() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {                     // If a new client connects,
    Serial.println("New Client.");  // print a message out in the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {  // if there's bytes to read from the client,
        char c = client.read();  // read a byte, then
        Serial.write(c);         // print it out the serial monitor
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            printConnectedInfo();

            // turns the GPIOs on and off
            if (header.indexOf("GET /led/on") >= 0) {
              Serial.println("D5 led on");
              portD5State = "on";
              digitalWrite(pin14, HIGH);

            } else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("D5 led off");
              portD5State = "off";
              digitalWrite(pin14, LOW);
            } else if (header.indexOf("GET /connect") >= 0) {
              Serial.printf("WiFi %s\n", connectData.ssid);
              Serial.printf("WiFi %s\n", connectData.pass);
            } else if (header.indexOf("GET /memory") >= 0) {
              Serial.println("Memory handler\nRead memory");
              memory.clearString();
              memory.writeString(connectData.ssid, 0);
              memory.readString(0);
              Serial.printf("Memory data %s\n", memory.buffer);

              memory.clearBuffer();
              memory.readString(5);
              Serial.printf("Read in 5 position data %s\n", memory.buffer);
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color: darkgray;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #393F40;}</style></head>");
            client.println("<body><h1>IOT Web Server</h1>");
            client.println("<p>Port D5 - Led " + portD5State + "</p>");
            if (portD5State == "off") {
              client.println("<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>");
              dsp.displayPrintText((char *)"\nPort D5 - Led Off");
            } else {
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>");
              dsp.displayPrintText((char *)"\nPort D5 - Led On");
            }
            client.println("</body></html>");
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }

    header = "";

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void printConnectedInfo() {
  dsp.displayClear();
  dsp.displayPrintText((char *)"WiFi connected.");
  dsp.displayPrintText((char *)connectData.ssid);
  dsp.displayPrintText((char *)"IP address: ");
  dsp.displayPrintText(WiFi.localIP());
}
