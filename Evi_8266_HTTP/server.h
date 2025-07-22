#ifndef _SERVER_H
#define _SERVER_H
#include "display.h"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

static ESP8266WebServer server(80);

void showInfo();
void actionOn();
void actionOff();
void motion();

#endif