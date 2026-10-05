// ============================================================
// DisplayService — управление дисплеем ST7789 (TTGO T-Display)
// ============================================================
// ИЗМЕНЕНО: добавлена обработка левой кнопки (GPIO0) —
//           долгое удержание (BUTTON_RESET_HOLD_MS) сбрасывает
//           все настройки (Wi-Fi + MQTT) и перезагружает плату.
// ИЗМЕНЕНО: во время удержания на экране показывается счётчик
//           секунд с предложением отпустить для отмены.
// ============================================================

#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <Arduino.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include "qrcode.h"

#include "config.h"
#include "StateManager.h"
#include "WiFiService.h"
#include "MqttService.h"
#include "LampStateService.h"

class DisplayService {
public:
  DisplayService(StateManager& stateManager,
                 WiFiService& wifiService,
                 MqttService& mqttService,
                 LampStateService& lampStateService);

  bool begin();
  void update();

private:
  enum class DisplayView {
    LAMP,
    SERVICE
  };

  StateManager& _stateManager;
  WiFiService& _wifiService;
  MqttService& _mqttService;
  LampStateService& _lampStateService;

  TFT_eSPI _tft;

  QRCode _qr;
  uint8_t _qrData[AP_QR_BUFFER_SIZE];

  unsigned long _lastUpdate;
  unsigned long _updateInterval;

  // --- Состояние видов (правая кнопка) ---
  DisplayView _view;
  unsigned long _serviceViewUntil;
  bool _lastButtonState;
  unsigned long _lastButtonChange;

  // === НОВОЕ: кэш состояния Wi-Fi для переключения AP ↔ Client
  // в update(). Без этого при старте в AP-режиме первый update()
  // рисовал Lamp view, а QR появлялся только после нажатия кнопки.
  bool _lastWifi;

  bool _lastLampMqtt;
  bool _lastLampHasData;
  String _lastLampPayload;

  // === НОВОЕ: сброс по левой кнопке ===
  bool _resetHoldActive;             // идёт удержание
  unsigned long _resetHoldStart;     // millis() начала удержания
  int _lastResetSecondShown;         // для обновления счётчика раз в секунду
  bool _resetButtonRaw;              // для антидребезга
  unsigned long _resetButtonChange;  // момент последнего изменения кнопки

  void drawScreen();
  void drawApMode();
  void drawLampView();
  void drawServiceView();
  void drawLampIcon(bool isOn);
  void drawLampUnknown();
  void drawError();
  void drawConnecting();
  void drawScanning();
  void drawQrCode();

  // === НОВОЕ: обработка кнопок и сброс ===
  void updateButton();
  void updateResetButton(unsigned long now);
  void drawResetHoldScreen(int secondsLeft);
  void performFactoryReset();

  void handleStateChange(AppStateEnum newState);

  AppStateEnum _currentState;

  bool _fullRedraw;
};

#endif