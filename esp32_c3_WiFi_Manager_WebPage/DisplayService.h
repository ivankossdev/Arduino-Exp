#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"

// Предварительное объявление класса AppState для избежания циклической зависимости
class AppState;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

class DisplayService {
public:
    // Конструктор теперь принимает ссылку на AppState для доступа к состоянию лампы
    DisplayService(StateManager& stateManager, WiFiService& wifiService,
                   MqttService& mqttService, AppState& appState);

    bool begin();
    void update();

private:
    StateManager& _stateManager;
    WiFiService& _wifiService;
    MqttService& _mqttService;
    AppState& _appState;          // Ссылка на главный объект приложения
    Adafruit_SSD1306 _display;

    unsigned long _lastUpdate;
    unsigned long _updateInterval;

    void drawScreen();
    void drawApMode();
    void drawClientMode();
    void drawError();
    void drawConnecting();
    void drawScanning();

    void setState(AppStateEnum newState);
    void handleStateChange(AppStateEnum newState);

    AppStateEnum _currentState;
};

#endif