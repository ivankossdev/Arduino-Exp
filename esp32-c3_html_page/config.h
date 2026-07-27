#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

extern const char* ssid;
extern const char* password;
extern const int ledPin;

extern const int DEFAULT_TEMPERATURE;
extern const unsigned long NONBLOCK_DELAY_MS;
extern const unsigned long WIFI_TIMEOUT_MS;

#endif
