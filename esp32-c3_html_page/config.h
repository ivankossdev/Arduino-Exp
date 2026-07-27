#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// Wi-Fi настройки
extern const char* ssid;
extern const char* password;

// Пин светодиода
extern const int ledPin;

// Глобальные объекты
extern WebServer server;
extern String ledState;

#endif