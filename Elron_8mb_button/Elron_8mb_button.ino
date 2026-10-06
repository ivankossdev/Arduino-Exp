#include "EventTimer.h"

// ==================== ПАРАМЕТРЫ ====================
const unsigned long BLINK_INTERVAL_SLOW = 1000;
const unsigned long BLINK_INTERVAL_FAST = 250;
const int           BLINK_COUNT         = 100;

const unsigned long DEBOUNCE_MS   = 50;    // окно антидребезга
const unsigned long LONG_PRESS_MS = 2000;  // порог «долгого» удержания

// Полярность кнопки на ACE-UNO (проверено экспериментально):
//   отпущена -> LOW
//   нажата   -> HIGH
const int BTN_ACTIVE = HIGH;
const int BTN_IDLE   = LOW;

// ==================== СОСТОЯНИЕ ====================
enum Mode { SLOW, FAST, OFF };

EventTimer blinkTimer(BLINK_INTERVAL_SLOW, BLINK_COUNT);
Mode mode = SLOW;

// --- Кнопка ---
bool          rawReading     = BTN_IDLE;
bool          stableReading  = BTN_IDLE;
unsigned long lastRawChange  = 0;
unsigned long pressStart     = 0;
bool          longPressFired = false;

// ==================== КОЛБЭКИ ТАЙМЕРА ====================

void onBlink(bool state, int count) {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
  Serial.print(mode == SLOW ? "SLOW " : "FAST ");
  Serial.print("Count ");
  Serial.println(count);
}

void onSeriesComplete() {
  Serial.println();
}

// ==================== ДЕЙСТВИЯ ====================

void switchMode() {
  switch (mode) {
    case SLOW:
      mode = FAST;
      blinkTimer.setInterval(BLINK_INTERVAL_FAST);
      Serial.println("Mode -> FAST");
      break;

    case FAST:
      mode = OFF;
      blinkTimer.stop();
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Mode -> OFF");
      break;

    case OFF:
      mode = SLOW;
      blinkTimer.setInterval(BLINK_INTERVAL_SLOW);
      blinkTimer.start();
      Serial.println("Mode -> SLOW");
      break;
  }
}

void resetAll() {
  mode = SLOW;
  blinkTimer.reset();                        // count = 0, state = false
  blinkTimer.setInterval(BLINK_INTERVAL_SLOW);
  blinkTimer.start();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Mode -> RESET");
}

// ==================== ОБРАБОТКА КНОПКИ ====================

void handleButton() {
  unsigned long now = millis();
  bool reading = digitalRead(BTN_BUILTIN);

  // 1. Запоминаем момент последнего изменения «сырого» сигнала
  if (reading != rawReading) {
    rawReading = reading;
    lastRawChange = now;
  }

  // 2. Принимаем новое состояние, только если оно стабильно > DEBOUNCE_MS
  if (reading != stableReading && (now - lastRawChange) >= DEBOUNCE_MS) {
    stableReading = reading;

    if (stableReading == BTN_ACTIVE) {
      // Нажатие
      pressStart     = now;
      longPressFired = false;
      Serial.println("[BTN] down");
    } else {
      // Отпускание
      unsigned long held = now - pressStart;
      if (!longPressFired && held < LONG_PRESS_MS) {
        switchMode();          // короткое нажатие -> смена режима
      }
      longPressFired = false;
    }
  }

  // 3. Долгое удержание — срабатывает ровно на 2-й секунде,
  //    пока палец ещё на кнопке
  if (stableReading == BTN_ACTIVE && !longPressFired && pressStart != 0
      && (now - pressStart) >= LONG_PRESS_MS) {
    longPressFired = true;
    resetAll();
  }
}

// ==================== SETUP / LOOP ====================

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BTN_BUILTIN, INPUT);   // без PULLUP — см. пояснение ниже

  // Диагностика полярности: кнопка точно не нажата в момент старта
  delay(50);
  Serial.print("BTN idle reading: ");
  Serial.println(digitalRead(BTN_BUILTIN) ? "HIGH" : "LOW");

  blinkTimer.setCallback(onBlink);
  blinkTimer.setSeriesCompleteCallback(onSeriesComplete);

  Serial.println("Mode -> SLOW");
}

void loop() {
  blinkTimer.tick();
  handleButton();
}