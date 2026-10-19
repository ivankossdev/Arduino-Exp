#pragma once

// ---------- OLED ----------
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

// ---------- I2C ----------
#define I2C_SDA 8
#define I2C_SCL 9

// ---------- UART ----------
#define UART_BAUD 9600
#define UART_RX   20
#define UART_TX   21
#define UART_BUF_LEN 128

// ---------- Кнопки ----------
#define BTN1_PIN   4
#define BTN2_PIN   5
#define BTN_DEBOUNCE_MS 30
#define BTN_ACTIVE_LOW  true   // кнопка на GND, резистор на 3.3V

// ---------- Лог ----------
#define MAX_LINES           8
#define LINE_HEIGHT         8
#define CHAR_WIDTH          6
#define MAX_CHARS_PER_LINE  (SCREEN_WIDTH / CHAR_WIDTH)
#define MAX_LINE_LEN        (MAX_CHARS_PER_LINE + 1)

// ---------- Heartbeat ----------
#define HB_PERIOD_MS 10000UL

// ---------- UART watchdog ----------
// Если от источника нет байт дольше этого времени — тревога.
// Должен быть БОЛЬШЕ максимальной паузы между сообщениями источника.
#define UART_TIMEOUT_MS  10000UL

// ---------- Команды на вторую плату ----------
// Отправляются как "<cmd>\n" (перевод строки как в мониторе)
#define CMD_BTN1  "show"