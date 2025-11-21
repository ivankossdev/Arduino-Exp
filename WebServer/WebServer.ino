#include "set.h"
#include <string>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

void clientHandler();
void printConnectedInfo();

unsigned long previousMillis = 0;
const long interval = 500;
void millisA0Read();
void clientAction(String url);
bool alarmStatus = false;

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
  /* Чтение названия точки доступа из памяти устройства */
  memory.clearBuffer();
  memory.readString(ENTERSSID);
  connectData.enterSSID(memory.buffer);

  /* Чтение пароля из памяти устройства*/
  memory.clearBuffer();
  memory.readString(ENTERPASS);
  connectData.enterPASS(memory.buffer);

  dsp.displayClear();
  dsp.displayPrintText((char *)"Connetcting to WiFi...");
  Serial.println("Connetcting to \nWiFi...");

  /* Подключение к сети WiFi */

  /*-------------Сделать функцию-----------------------------*/
  IPAddress ip(192, 168, 0, 60);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gateway(192, 168, 0, 1);
  WiFi.config(ip, subnet, gateway);
  /*---------------------------------------------------------*/

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
      memory.writeString(connectData.ssid, ENTERSSID);
      memory.writeString(connectData.pass, ENTERPASS);
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

  /* Адрес сервера обработчика событий */
  connectData.urlClientAction = "http://192.168.0.175:10500/action"; 
}

void loop() {
  if (!stateConnection) {
    clientHandler();
  }

  menu.menu();

  if (menu.isSetIP) {
    menu.isSetIP = false;
    printConnectedInfo();
  }
  millisA0Read();
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
            } else if (header.indexOf("GET /action/alarm") >= 0) {
              clientAction(connectData.urlClientAction);
            }

            client.println("<!DOCTYPE html><html lang=\"ru\">");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color: darkgray;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; width: 300px;}");
            client.println(".button2 {background-color: #393F40;}</style></head>");
            client.println("<body><h1>Evidence Securiy Controller</h1>");
            client.println("<p>Connected to the network " + mySSID + "</p>");
            client.println("<p>Relay 1  - " + portD5State + "</p>");

            if (portD5State == "off") {
              client.println("<p><a href=\"/led/on\"><button class=\"button\">Relay ON</button></a></p>");
              dsp.displayPrintText((char *)"\nRelay 1 - Off");
              if (alarmStatus) {
                dsp.displayPrintText((char *)"\nAlarm Zone 1");
              }
            } else {
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">Relay OFF</button></a></p>");
              dsp.displayPrintText((char *)"\nRelay 1 - On");
              if (alarmStatus) {
                dsp.displayPrintText((char *)"\nAlarm Zone 1");
              }
            }
            client.println("</body></html>");
            client.println();
            client.println("<p><a href=\"/action/alarm\"><button class=\"button\">Alarm</button></a></p>");
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
  Serial.printf("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  /* Данные на дисплее */
  dsp.displayClear();
  dsp.displayPrintText((char *)"WiFi connected.");
  dsp.displayPrintText((char *)connectData.ssid);
  dsp.displayPrintText((char *)"IP address: ");
  dsp.displayPrintText(WiFi.localIP());
}

/* Обработчик аналового порта */
void millisA0Read() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int portA0 = analogRead(A0);
    if (portA0 > 550 && portA0 <= 1024) {
      if (!alarmStatus) {
        alarmStatus = true;
        Serial.printf("A0 = %d\n", analogRead(A0));
        dsp.displayClear();
        dsp.displayPrintText((char *)"WiFi connected.");
        dsp.displayPrintText((char *)connectData.ssid);
        dsp.displayPrintText((char *)"IP address: ");
        dsp.displayPrintText(WiFi.localIP());
        dsp.displayPrintText((char *)"\nAlarm Zone 1");
        clientAction(connectData.urlClientAction);
      }
    } else {
      if (alarmStatus) {
        alarmStatus = false;
        dsp.displayClear();
        dsp.displayPrintText((char *)"WiFi connected.");
        dsp.displayPrintText((char *)connectData.ssid);
        dsp.displayPrintText((char *)"IP address: ");
        dsp.displayPrintText(WiFi.localIP());
      }
    }
  }
}

/* HTTP клиент */
void clientAction(String url) {
  WiFiClient client;
  HTTPClient http;

  if (http.begin(client, url)) { 

    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
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
