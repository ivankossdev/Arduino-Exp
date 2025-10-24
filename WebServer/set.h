#ifndef _SET_H
#define _SET_H

#include <ESP8266WiFi.h>
#include "scanWiFi.h"
#include "display.h"
#include "MemHandler.h"
#include "connectData.h"
#include "ConnectHandler.h"
#include "serialHandler.h"

WiFiServer server(80);
Memory memory;
String header;
String portD5State = "off";
SerialHandler serialHandler; 

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;
bool stateConnection = false;

const int pin14 = 14;

#endif