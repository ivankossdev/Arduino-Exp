#include "display_log.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Кольцевой буфер
static char lines[MAX_LINES][MAX_LINE_LEN];
static int  head  = 0;
static int  count = 0;

// Статусная строка
static char statusText[MAX_LINE_LEN] = "";

static void addLine(const char *s) {
  strncpy(lines[head], s, MAX_CHARS_PER_LINE);
  lines[head][MAX_CHARS_PER_LINE] = '\0';
  head = (head + 1) % MAX_LINES;
  if (count < MAX_LINES) count++;
}

static void redraw() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // 1. Рисуем статус (строка 0)
  display.setCursor(0, 0);
  display.print(statusText);

  // 2. Рисуем разделитель (строка 1)
  display.setCursor(0, LINE_HEIGHT);
  display.print("--------------------");

  // 3. Рисуем лог (строки 2..7)
  int start = (head - count + MAX_LINES) % MAX_LINES;
  for (int i = 0; i < count; i++) {
    int idx = (start + i) % MAX_LINES;
    display.setCursor(0, (i + 2) * LINE_HEIGHT); // +2, чтобы пропустить статус и линию
    display.print(lines[idx]);
  }
  display.display();
}

void logInit() {
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while (true) delay(1000);
  }
}

void logLine(const char *s) {
  int len = (int)strlen(s);
  if (len == 0) {
    addLine("");
  } else {
    int pos = 0;
    while (pos < len) {
      char chunk[MAX_LINE_LEN];
      int n = (len - pos > MAX_CHARS_PER_LINE) ? MAX_CHARS_PER_LINE : (len - pos);
      memcpy(chunk, s + pos, n);
      chunk[n] = '\0';
      addLine(chunk);
      pos += n;
    }
  }
  redraw();
}

void statusLine(const char *s) {
  strncpy(statusText, s, MAX_CHARS_PER_LINE);
  statusText[MAX_CHARS_PER_LINE] = '\0';
  redraw();
}

void logClear() {
  head = 0;
  count = 0;
  statusText[0] = '\0';
  redraw();
}