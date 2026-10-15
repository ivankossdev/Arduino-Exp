/*
 * ESP32-C3 Super Mini + SSD1306 (I2C) + UART -> OLED
 * + heartbeat с состоянием памяти
 *
 * Без String: фиксированные буферы, кольцевой индекс.
 *
 * Подключение:
 *   SSD1306 SDA -> GPIO8
 *   SSD1306 SCL -> GPIO9
 *   UART RX     -> GPIO20
 *   UART TX     -> GPIO21   (можно не подключать)
 *   GND общий обязательно!
 *
 * Arduino IDE:
 *   Board: ESP32C3 Dev Module
 *   USB CDC On Boot: Enabled
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_heap_caps.h>

// ---------- OLED ----------
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

// ---------- I2C ----------
#define I2C_SDA 8
#define I2C_SCL 9

// ---------- UART ----------
#define UART_RX   20
#define UART_TX   21
#define UART_BAUD 9600

// ---------- Лог ----------
#define MAX_LINES           8
#define LINE_HEIGHT         8
#define CHAR_WIDTH          6
#define MAX_CHARS_PER_LINE  (SCREEN_WIDTH / CHAR_WIDTH)    // = 21
#define MAX_LINE_LEN        (MAX_CHARS_PER_LINE + 1)       // = 22

#define UART_BUF_LEN        128

// ---------- Heartbeat ----------
#define HB_PERIOD_MS        10000UL    // раз в 10 секунд

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Кольцевой буфер строк
char lines[MAX_LINES][MAX_LINE_LEN];
int  head  = 0;
int  count = 0;

// Буфер приёма UART
char uartBuf[UART_BUF_LEN];
int  uartLen = 0;
unsigned long lastCharTime = 0;
const unsigned long IDLE_TIMEOUT = 150;

// Heartbeat
unsigned long lastHB = 0;

// ---------------- Дисплей ----------------
void addDisplayLine(const char *s) {
  strncpy(lines[head], s, MAX_CHARS_PER_LINE);
  lines[head][MAX_CHARS_PER_LINE] = '\0';
  head = (head + 1) % MAX_LINES;
  if (count < MAX_LINES) count++;
}

void redraw() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int start = (head - count + MAX_LINES) % MAX_LINES;
  for (int i = 0; i < count; i++) {
    int idx = (start + i) % MAX_LINES;
    display.setCursor(0, i * LINE_HEIGHT);
    display.print(lines[idx]);
  }
  display.display();
}

void logLine(const char *s) {
  int len = (int)strlen(s);
  if (len == 0) {
    addDisplayLine("");
  } else {
    int pos = 0;
    while (pos < len) {
      char chunk[MAX_LINE_LEN];
      int n = (len - pos > MAX_CHARS_PER_LINE) ? MAX_CHARS_PER_LINE : (len - pos);
      memcpy(chunk, s + pos, n);
      chunk[n] = '\0';
      addDisplayLine(chunk);
      pos += n;
    }
  }
  redraw();
}

// ---------------- Setup / Loop ----------------
void setup() {
  Serial1.begin(UART_BAUD, SERIAL_8N1, UART_RX, UART_TX);

  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while (true) delay(1000);
  }

  logLine("System ready");

  lastHB = millis();
}

void flushUartMessage() {
  uartBuf[uartLen] = '\0';

  char msg[UART_BUF_LEN + 8];
  snprintf(msg, sizeof(msg), "[UART] %s", uartBuf);
  logLine(msg);

  uartLen = 0;
}

void heartbeat() {
  unsigned long up_s   = millis() / 1000UL;
  unsigned long hfree  = ESP.getFreeHeap() / 1024UL;
  unsigned long hlarge = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT) / 1024UL;

  char l1[24];
  char l2[24];
  snprintf(l1, sizeof(l1), "[HB] up=%lus", up_s);
  snprintf(l2, sizeof(l2), "h=%luk lg=%luk", hfree, hlarge);
  logLine(l1);
  logLine(l2);
}

void loop() {
  // --- Приём с UART ---
  while (Serial1.available()) {
    char c = (char)Serial1.read();
    lastCharTime = millis();

    if (c == '\n') {
      flushUartMessage();
    } else if (c != '\r') {
      if (uartLen >= UART_BUF_LEN - 1) {
        flushUartMessage();
      }
      uartBuf[uartLen++] = c;
    }
  }

  if (uartLen > 0 && (millis() - lastCharTime > IDLE_TIMEOUT)) {
    flushUartMessage();
  }

  // --- Heartbeat ---
  if (millis() - lastHB >= HB_PERIOD_MS) {
    lastHB = millis();
    heartbeat();
  }
}