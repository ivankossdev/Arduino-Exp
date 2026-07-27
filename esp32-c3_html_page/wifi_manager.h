#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

enum WifiState {
  WIFI_NOT_STARTED,
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_FAILED
};

class WifiManager {
public:
  WifiManager(const char* ssid, const char* pass)
    : _ssid(ssid), _password(pass), _state(WIFI_NOT_STARTED) {}

  void begin() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);
    _state = WIFI_CONNECTING;
    _startTime = millis();
    _loggedOnce = false;
    Serial.print("Подключение к Wi-Fi");
  }

  void tick() {
    if (_state != WIFI_CONNECTING) return;

    unsigned long now = millis();

    static unsigned long lastDotTime = 0;
    if (now - lastDotTime >= 500) {
      Serial.print(".");
      lastDotTime = now;
    }

    if (WiFi.status() == WL_CONNECTED) {
      _state = WIFI_CONNECTED;
      Serial.println();
      Serial.println("Подключено к Wi‑Fi!");
      Serial.print("IP-адрес: ");
      Serial.println(WiFi.localIP());
    } else if (now - _startTime >= WIFI_TIMEOUT_MS) {
      _state = WIFI_FAILED;
      Serial.println();
      Serial.println("Не удалось подключиться к Wi‑Fi за таймаут.");
    }
  }

  bool isConnected() const { return _state == WIFI_CONNECTED; }
  bool hasFailed() const { return _state == WIFI_FAILED; }

private:
  const char* _ssid;
  const char* _password;
  WifiState _state;
  unsigned long _startTime;
  bool _loggedOnce;
};

#endif
