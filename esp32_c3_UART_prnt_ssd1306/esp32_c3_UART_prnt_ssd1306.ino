/*
 * ESP32-C3 Super Mini + SSD1306 + UART -> OLED
 * + heartbeat + 2 кнопки (внешняя подтяжка 10k к 3.3V)
 *
 * Подключение:
 *   SSD1306 SDA -> GPIO8
 *   SSD1306 SCL -> GPIO9
 *   UART RX     -> GPIO20
 *   UART TX     -> GPIO21   (можно не подключать)
 *   BTN1: 3.3V --[10k]-- GPIO4 --[кнопка]-- GND
 *   BTN2: 3.3V --[10k]-- GPIO5 --[кнопка]-- GND
 *
 * Arduino IDE:
 *   Board: ESP32C3 Dev Module
 *   USB CDC On Boot: Enabled
 */

#include "config.h"
#include "display_log.h"
#include "uart_receiver.h"
#include "buttons.h"
#include "heartbeat.h"

static void onUartLine(const char *line) {
  char msg[UART_BUF_LEN + 8];
  snprintf(msg, sizeof(msg), "[UART] %s", line);
  logLine(msg);
}

void setup() {
  logInit();
  buttonsInit();
  uartInit(UART_BAUD, UART_RX, UART_TX, onUartLine);
  heartbeatInit(HB_PERIOD_MS);

  logLine("System ready");
}

void loop() {
  switch (buttonsPoll()) {
    case BTN1_PRESSED: logClear();         break;
    case BTN2_PRESSED: heartbeatShowNow(); break;
    default: break;
  }

  uartPoll();
  heartbeatPoll();
}