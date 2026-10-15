/*
 * ESP32-C3 Super Mini + SSD1306 + UART -> OLED
 * + heartbeat + 2 кнопки (внешняя подтяжка резистором 10k)
 *
 * Подключение:
 *   SSD1306 SDA -> GPIO8
 *   SSD1306 SCL -> GPIO9
 *   UART RX     -> GPIO20
 *   UART TX     -> GPIO21   (можно не подключать)
 *
 *   BTN1: 3.3V --[10k]--+-- GPIO4 --[кнопка]-- GND
 *   BTN2: 3.3V --[10k]--+-- GPIO5 --[кнопка]-- GND
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

// ---------- Кнопки ----------
#define BTN1_PIN   4
#define BTN2_PIN   5
#define BTN_DEBOUNCE_MS 30

// true  -> кнопка к GND, подтяжка к 3.3V (нажатие = LOW)   <-- по умолчанию
// false -> кнопка к 3.3V, подтяжка к GND  (нажатие = HIGH)
#define BTN_ACTIVE_LOW  true

// ---------- Лог ----------
#define MAX_LINES           8
#define LINE_HEIGHT         8
#define CHAR_WIDTH          6
#define MAX_CHARS_PER_LINE  (SCREEN_WIDTH / CHAR_WIDTH)    // = 21
#define MAX_LINE_LEN        (MAX_CHARS_PER_LINE + 1)       // = 22

#define UART_BUF_LEN        128

// ---------- Heartbeat ----------
#define HB_PERIOD_MS        10000UL

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

// Кнопки
struct Button {
  uint8_t  pin;
  bool     stable;       // стабильное состояние (true = нажата)
  bool     lastRead;     // последнее логическое чтение
  unsigned long lastChange;
};

Button btn1 = { BTN1_PIN, false, false, 0 };
Button btn2 = { BTN2_PIN, false, false, 0 };

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

// ---------------- Действия кнопок ----------------
void clearDisplay() {
  head = 0;
  count = 0;
  redraw();
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

// ---------------- Кнопки ----------------
bool buttonPressed(Button &b) {
#if BTN_ACTIVE_LOW
  bool pressed = (digitalRead(b.pin) == LOW);
#else
  bool pressed = (digitalRead(b.pin) == HIGH);
#endif

  if (pressed != b.lastRead) {
    b.lastRead = pressed;
    b.lastChange = millis();
  }

  if (millis() - b.lastChange >= BTN_DEBOUNCE_MS) {
    if (pressed != b.stable) {
      b.stable = pressed;
      if (b.stable) return true;
    }
  }
  return false;
}

void pollButtons() {
  if (buttonPressed(btn1)) clearDisplay();
  if (buttonPressed(btn2)) heartbeat();
}

// ---------------- Setup / Loop ----------------
void setup() {
  // Внешняя подтяжка — INPUT, не INPUT_PULLUP
  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);

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

void loop() {
  pollButtons();

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

  if (millis() - lastHB >= HB_PERIOD_MS) {
    lastHB = millis();
    heartbeat();
  }
}