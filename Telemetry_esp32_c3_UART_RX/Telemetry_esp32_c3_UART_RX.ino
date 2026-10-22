#include "config.h"
#include "display_log.h"
#include "uart_receiver.h"
#include "uart_sender.h"
#include "uart_watchdog.h"
#include "buttons.h"
#include "heartbeat.h"
#include "telemetry.h"
#include "stats.h"

static void onUartLine(const char *line) {
  telemetryHandle(line);
}

void setup() {
  Serial.begin(115200); // USB порт для CLI
  logInit();
  statsInit();
  buttonsInit();
  uartInit(UART_BAUD, UART_RX, UART_TX, onUartLine);
  uartWatchdogInit(UART_TIMEOUT_MS);
  heartbeatInit(HB_PERIOD_MS);

  logLine("System ready");
}

void loop() {
  // 1. Обработка кнопок
  switch (buttonsPoll()) {
    case BTN1_PRESSED:
      uartSendReliable(CMD_BTN1); 
      break;
    case BTN2_PRESSED:
      uartSendReliable("stat");   
      break;
    default:
      break;
  }

  // 2. Обработка команд из USB Serial (CLI)
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() > 0) {
      uartSendReliable(cmd.c_str());
      Serial.print("Sent: ");
      Serial.println(cmd);
    }
  }

  // 3. Системные опросы
  uartPoll();
  uartRetryPoll(); 
  uartWatchdogPoll();
  heartbeatPoll();
}