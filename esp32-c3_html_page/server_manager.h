#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <WebServer.h>
#include <functional>
#include "fs_utils.h"

using PlaceholderProcessor = String (*)(const String&);

class ServerManager {
public:
  explicit ServerManager(WebServer& srv) : _server(srv) {}

  void setupRoutes(PlaceholderProcessor processor) {
    _server.on("/", HTTP_GET, [this, processor]() {
      String content = getFileContent("/index.html");
      if (content.isEmpty()) {
        _server.send(404, "text/plain", "File not found");
        return;
      }
      content.replace("%STATE%", processor("STATE"));
      content.replace("%TEMPERATURE%", processor("TEMPERATURE"));
      content.replace("%HUMIDITY%", processor("HUMIDITY"));
      _server.send(200, "text/html", content);
    });

    _server.on("/style.css", HTTP_GET, [this]() {
      sendFile(_server, "/style.css", "text/css");
    });

    _server.on("/script.js", HTTP_GET, [this]() {
      sendFile(_server, "/script.js", "application/javascript");
    });

    _server.on("/data", HTTP_GET, [this]() {
      extern int temperature;
      extern const int ledPin;
      String json = "{";
      json += "\"temperature\":" + String(temperature) + ",";
      json += "\"humidity\":60.0,";
      json += "\"state\":\"" + String(digitalRead(ledPin) ? "OFF" : "ON") + "\"";
      json += "}";
      _server.send(200, "application/json", json);
    });

    _server.on("/on", HTTP_GET, [&]() {
      extern const int ledPin;
      digitalWrite(ledPin, LOW);
      _server.send(200, "text/plain", "OK");
    });

    _server.on("/off", HTTP_GET, [&]() {
      extern const int ledPin;
      digitalWrite(ledPin, HIGH);
      _server.send(200, "text/plain", "OK");
    });
  }

  void start() {
    _server.begin();
    Serial.println("HTTP-сервер запущен.");
  }

  void tick() {
    _server.handleClient();
  }

private:
  WebServer& _server;
};

#endif  // <-- Вот он Обязательно должен быть в конце
