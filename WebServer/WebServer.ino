#include "set.h"
#include <string>

void clientHandler();
void printConnectedInfo();
void serialReader();

void setup() {
  Serial.begin(115200);
  dsp.displayInit();

  /* Инициализация портов */
  pinMode(pin14, OUTPUT);
  digitalWrite(pin14, HIGH);

  /* Сканирование доступный сетей */
  dsp.displayPrintText((char *)"Scan WiFi networks");
  searchWiFi();

  /* Авторизайия в сети WiFi */
  /* Чтение названия точки доступа из памяти устройства*/
  memory.clearBuffer();
  memory.readString(0);
  connectData.enterSSID(memory.buffer);

  /* Чтение пароля из памяти устройства*/
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
    /* Повторное подключение в случае ошибки подключения */
    do {
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

      stateConnection = false;
      waitConnect(&stateConnection);
    } while (stateConnection);

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
  serialReader();
}

void serialReader() {
  if (Serial.available() > 0) {
    String serialData = Serial.readString();
    int i = 0;
    do {
      if (serialData[i] == '\r' || serialData[i] == '\n') break;
      Serial.printf("%c", serialData[i]);
      i++;
    } while (serialData[i] != '\0');
    Serial.printf("\n");
  }
}

void clientHandler() {

  WiFiClient client = server.available();
  String mySSID(connectData.ssid);

  if (client) {
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;

    Serial.println("");

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();

      if (client.available()) {
        char c = client.read();

        Serial.write(c);

        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            printConnectedInfo();

            if (header.indexOf("GET /led/on") >= 0) {
              Serial.println("Relay 1 on");
              portD5State = "on";
              digitalWrite(pin14, LOW);
            } else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("Relay 1 off");
              portD5State = "off";
              digitalWrite(pin14, HIGH);
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color: darkgray;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #393F40;}</style></head>");
            client.println("<body><h1>Evidence Securiy Controller</h1>");
            client.println("<p>Connected to the network " + mySSID + "</p>");
            client.println("<p>Relay 1  - " + portD5State + "</p>");

            if (portD5State == "off") {
              client.println("<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>");
              dsp.displayPrintText((char *)"\nRelay 1 - Off");
            } else {
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>");
              dsp.displayPrintText((char *)"\nRelay 1 - On");
            }
            client.println("</body></html>");
            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    mySSID = "";

    client.stop();
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
