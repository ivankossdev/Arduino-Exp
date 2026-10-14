/*
 * ESP32-C3 Super Mini + SSD1306 (I2C) + UART -> OLED (лог в столбик)
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

// ---------- Лог на дисплее ----------
#define MAX_LINES           8
#define LINE_HEIGHT         8
#define CHAR_WIDTH          6
#define MAX_CHARS_PER_LINE  (SCREEN_WIDTH / CHAR_WIDTH)   // = 21

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String lines[MAX_LINES];
int    lineCount = 0;

String uartBuffer = "";
unsigned long lastCharTime = 0;
const unsigned long IDLE_TIMEOUT = 150;

// ---------------- Дисплей ----------------
void redraw() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  for (int i = 0; i < lineCount; i++) {
    display.setCursor(0, i * LINE_HEIGHT);
    display.print(lines[i]);
  }
  display.display();
}

void addDisplayLine(const String &s) {
  if (lineCount < MAX_LINES) {
    lines[lineCount++] = s;
  } else {
    for (int i = 1; i < MAX_LINES; i++) lines[i - 1] = lines[i];
    lines[MAX_LINES - 1] = s;
  }
}

void appendLog(const String &s) {
  if (s.length() <= MAX_CHARS_PER_LINE) {
    addDisplayLine(s);
  } else {
    int pos = 0;
    while (pos < (int)s.length()) {
      addDisplayLine(s.substring(pos, pos + MAX_CHARS_PER_LINE));
      pos += MAX_CHARS_PER_LINE;
    }
  }
  redraw();
}

// Единая точка логирования (только на экран)
void logLine(const String &s) {
  // Serial.println(s);   // <-- отладка отключена
  appendLog(s);
}

// ---------------- Setup / Loop ----------------
void setup() {
  // --- Отладка через USB (отключена) ---
  // Serial.begin(115200);
  // unsigned long t0 = millis();
  // while (!Serial && millis() - t0 < 3000) delay(10);
  // Serial.println();
  // Serial.println("=== BOOT ===");

  // --- UART для приёма ---
  Serial1.begin(UART_BAUD, SERIAL_8N1, UART_RX, UART_TX);
  // Serial.printf("Serial1 started @ %d baud (RX=%d, TX=%d)\n",
  //               UART_BAUD, UART_RX, UART_TX);

  // --- I2C + OLED ---
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Serial.println("SSD1306 not found!");
    while (true) delay(1000);
  }

  logLine("System ready");
}

void loop() {
  // --- Приём с UART ---
  while (Serial1.available()) {
    char c = (char)Serial1.read();
    lastCharTime = millis();

    if (c == '\n') {
      logLine("[UART] " + uartBuffer);
      uartBuffer = "";
    } else if (c != '\r') {
      uartBuffer += c;
      if (uartBuffer.length() > 200) {
        logLine("[UART] " + uartBuffer);
        uartBuffer = "";
      }
    }
  }

  // Если строка пришла без '\n' — выводим по паузе
  if (uartBuffer.length() > 0 && (millis() - lastCharTime > IDLE_TIMEOUT)) {
    logLine("[UART] " + uartBuffer);
    uartBuffer = "";
  }

  // --- Периодический tick (отключён) ---
  // static unsigned long lastTick = 0;
  // if (millis() - lastTick > 5000) {
  //   lastTick = millis();
  //   char buf[64];
  //   snprintf(buf, sizeof(buf), "[tick] uptime=%lu ms, free heap=%u",
  //            millis(), (unsigned)ESP.getFreeHeap());
  //   logLine(String(buf));
  // }
}