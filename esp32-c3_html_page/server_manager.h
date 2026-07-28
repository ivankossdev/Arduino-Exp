#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <WebServer.h>
#include <functional>
#include "fs_utils.h"
#include "temperature_manager.h"

using PlaceholderProcessor = String (*)(const String&);

class ServerManager {
public:
  // Теперь передаём и сервер, и менеджер температуры
  explicit ServerManager(WebServer& srv, TemperatureManager& tMgr)
    : _server(srv), _tempMgr(tMgr) {}

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

    // Читаем температуру напрямую из _tempMgr — никакой глобальной переменной!
    _server.on("/data", HTTP_GET, [&]() {
      extern const int ledPin;
      String json = "{";
      json += "\"temperature\":" + String(_tempMgr.get()) + ",";
      json += "\"humidity\":60.0,";
      String ledStateStr = (digitalRead(ledPin) == HIGH) ? "OFF" : "ON";
      json += "\"state\":\"" + ledStateStr + "\"";
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
  TemperatureManager& _tempMgr;  // <-- Храним ссылку на менеджер
};

#endif
