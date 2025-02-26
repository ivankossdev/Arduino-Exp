#ifndef _WIFI_H
#define _WIFI_H
#include <ESP8266WiFi.h>

#define STASSID "HUAWEI-V4XQZZ_HiLink"
#define STAPSK "12345678"

const char* ssid = STASSID;
const char* password = STAPSK;

void wifiInit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

#endif