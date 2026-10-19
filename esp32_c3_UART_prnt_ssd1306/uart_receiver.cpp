#include "uart_receiver.h"
#include "config.h"

static char buf[UART_BUF_LEN];
static int  len = 0;
static unsigned long lastCharTime = 0;
static const unsigned long IDLE_TIMEOUT = 150;

static UartLineCallback callback = nullptr;

static void flush() {
  buf[len] = '\0';
  if (callback) callback(buf);
  len = 0;
}

void uartInit(uint32_t baud, int rx, int tx, UartLineCallback cb) {
  callback = cb;
  Serial1.begin(baud, SERIAL_8N1, rx, tx);
  lastCharTime = millis();
}

void uartPoll() {
  while (Serial1.available()) {
    char c = (char)Serial1.read();
    lastCharTime = millis();

    if (c == '\n') {
      flush();
    } else if (c != '\r') {
      if (len >= UART_BUF_LEN - 1) flush();
      buf[len++] = c;
    }
  }

  if (len > 0 && (millis() - lastCharTime > IDLE_TIMEOUT)) flush();
}

unsigned long uartLastActivity() {
  return lastCharTime;
}