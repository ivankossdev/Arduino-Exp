#include "set.h"

void clientHandler();
void printConnectedInfo();

void setup() {
  Serial.begin(115200);
  dsp.displayInit();

  /* Инициализация портов */
  pinMode(pin14, OUTPUT);
  digitalWrite(pin14, LOW);

  /* Сканирование доступный сетей */
  dsp.displayPrintText((char *)"Scan WiFi networks");
  searchWiFi();

  /* Авторизайия в сети WiFi */
  /* Чтение названия точки доступа из памяти устройства*/
  memory.clearBuffer();
  memory.readString(0);
  connectData.enterSSID(memory.buffer);

  /* Чтение пароля из памяти из памяти устройства*/
  memory.clearBuffer();
  memory.readString(32);
  connectData.enterPASS(memory.buffer);

  dsp.displayClear();
  dsp.displayPrintText((char *)"Connetcting to WiFi...");
  Serial.println("Connetcting to \nWiFi...");

  /* Подключение к сети WiFi */
  Serial.print("Connecting to ");
  Serial.println(connectData.ssid);
  WiFi.begin(connectData.ssid, connectData.pass);

  waitConnect(&stateConnection);

  if (stateConnection) {
    Serial.println("");
    Serial.println("Error WiFi connected.");

    dsp.displayClear();
    dsp.displayPrintText((char *)"Error WiFi connected.");

    /* Авторизация из консоли */
    delay(1000);

    dsp.displayClear();
    dsp.displayPrintText((char *)"Please enter WiFi network: ");
    connectData.enterSSID();

    dsp.displayClear();
    dsp.displayPrintText((char *)"Please enter password: ");
    connectData.enterPASS();

    /* Запись в память логин и пароль */
    memory.clearAllMemory();
    memory.writeString(connectData.ssid, 0);
    memory.writeString(connectData.pass, 32);
    WiFi.begin(connectData.ssid, connectData.pass);

    waitConnect(&stateConnection);

    stateConnection = false;
    server.begin();
    /* Данные о подключении */
    printConnectedInfo();
  } else {
    server.begin();
    /* Данные о подключении */
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
            } else if (header.indexOf("GET /clear") >= 0) {
              memory.clearAllMemory();
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
  /* Данные в консоли */
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* Данные на дисплее */
  dsp.displayClear();
  dsp.displayPrintText((char *)"WiFi connected.");
  dsp.displayPrintText((char *)connectData.ssid);
  dsp.displayPrintText((char *)"IP address: ");
  dsp.displayPrintText(WiFi.localIP());
}


