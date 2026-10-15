/*
 * ESP32-C3 Super Mini + SSD1306 (I2C) + UART -> OLED (лог в столбик)
 *
 * Без String: фиксированные буферы + кольцевой индекс.
 * В loop() нет ни одного malloc/free — фрагментация кучи невозможна.
 *
 * Подключение:
 *   SSD1306 SDA -> GPIO8
 *   SSD1306 SCL -> GPIO9
 *   UART RX     -> GPIO20
 *   UART TX     -> GPIO21   (можно не подключать)
 *   GND общий обязательно!
 *
 * Arduino IDE (плата "ESP32C3 Dev Module"):
 *   Tools -> USB CDC On Boot -> Enabled
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
#define MAX_LINES           8                              // строк на экране
#define LINE_HEIGHT         8                              // пикселей на строку
#define CHAR_WIDTH          6                              // ширина символа (TextSize=1)
#define MAX_CHARS_PER_LINE  (SCREEN_WIDTH / CHAR_WIDTH)    // = 21
#define MAX_LINE_LEN        (MAX_CHARS_PER_LINE + 1)       // + '\0' = 22

#define UART_BUF_LEN        128                            // буфер приёма UART

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Кольцевой буфер строк
char lines[MAX_LINES][MAX_LINE_LEN];
int  head  = 0;      // куда писать следующую строку
int  count = 0;      // сколько строк реально занято (0..MAX_LINES)

// Буфер приёма UART
char uartBuf[UART_BUF_LEN];
int  uartLen = 0;
unsigned long lastCharTime = 0;
const unsigned long IDLE_TIMEOUT = 150;

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

  int start = (head - count + MAX_LINES) % MAX_LINES;  // самая старая строка
  for (int i = 0; i < count; i++) {
    int idx = (start + i) % MAX_LINES;
    display.setCursor(0, i * LINE_HEIGHT);
    display.print(lines[idx]);
  }
  display.display();
}

// Пишет одну логическую строку: режет на куски по 21 символу и кладёт в буфер
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
}

// Завершение приёма сообщения: добавляем префикс и логируем
void flushUartMessage() {
  uartBuf[uartLen] = '\0';

  char msg[UART_BUF_LEN + 8];
  snprintf(msg, sizeof(msg), "[UART] %s", uartBuf);
  logLine(msg);

  uartLen = 0;
}

void loop() {
  // --- Приём с UART ---
  while (Serial1.available()) {
    char c = (char)Serial1.read();
    lastCharTime = millis();

    if (c == '\n') {
      flushUartMessage();
    } else if (c != '\r') {
      // если буфер переполнен — сбрасываем как отдельное сообщение
      if (uartLen >= UART_BUF_LEN - 1) {
        flushUartMessage();
      }
      uartBuf[uartLen++] = c;
    }
  }

  // Сообщение без '\n' — по паузе
  if (uartLen > 0 && (millis() - lastCharTime > IDLE_TIMEOUT)) {
    flushUartMessage();
  }
}