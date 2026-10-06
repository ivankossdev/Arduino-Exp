#include "EventTimer.h"
#include "Settings.h"
#include "Button.h"

// ==================== КОНСТАНТЫ ПРИКЛАДНОГО УРОВНЯ ====================
const uint8_t BTN_ACTIVE = HIGH;   // на ACE-UNO кнопка активна по HIGH

// ==================== ОБЪЕКТЫ ====================
EventTimer blinkTimer(1000, 10);   // интервал/счёт перезапишутся из settings
Button     button(BTN_BUILTIN, BTN_ACTIVE, 50);

// ==================== КОЛБЭКИ ТАЙМЕРА ====================

void onBlink(bool state, int count) {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
  Serial.print(settings.mode == SLOW ? "SLOW " : "FAST ");
  Serial.print("Count ");
  Serial.println(count);
}

void onSeriesComplete() {
  Serial.println();
}

// ==================== ЛОГИКА РЕЖИМОВ ====================

void applySettings() {
  blinkTimer.setMaxIterations(settings.blinkCount);
  switch (settings.mode) {
    case SLOW:
      blinkTimer.setInterval(settings.intervalSlow);
      blinkTimer.start();
      break;
    case FAST:
      blinkTimer.setInterval(settings.intervalFast);
      blinkTimer.start();
      break;
    case OFF:
      blinkTimer.stop();
      digitalWrite(LED_BUILTIN, LOW);
      break;
  }
}

void switchMode() {
  switch (settings.mode) {
    case SLOW:
      settings.mode = FAST;
      blinkTimer.setInterval(settings.intervalFast);
      Serial.println("Mode -> FAST");
      break;
    case FAST:
      settings.mode = OFF;
      blinkTimer.stop();
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Mode -> OFF");
      break;
    case OFF:
      settings.mode = SLOW;
      blinkTimer.setInterval(settings.intervalSlow);
      blinkTimer.start();
      Serial.println("Mode -> SLOW");
      break;
  }
  settingsSave();
}

void resetAll() {
  settings.mode = SLOW;
  blinkTimer.reset();
  blinkTimer.setInterval(settings.intervalSlow);
  blinkTimer.start();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Mode -> RESET");
  settingsSave();
}

// ==================== SETUP / LOOP ====================

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BTN_BUILTIN, INPUT);

  settingsInit();   // EEPROM.begin() + загрузка + валидация

  delay(50);
  Serial.print("BTN idle reading: ");
  Serial.println(digitalRead(BTN_BUILTIN) ? "HIGH" : "LOW");

  // Таймер
  blinkTimer.setCallback(onBlink);
  blinkTimer.setSeriesCompleteCallback(onSeriesComplete);

  // Кнопка — настраивается после settingsInit(),
  // потому что longPressMs берётся из настроек
  button.setLongPressMs(settings.longPressMs);
  button.setShortPressCallback(switchMode);
  button.setLongPressCallback(resetAll);

  // Применяем восстановленный режим
  applySettings();

  Serial.print("Mode -> ");
  Serial.println(settings.mode == SLOW ? "SLOW" :
                 settings.mode == FAST ? "FAST" : "OFF");
}

void loop() {
  blinkTimer.tick();
  button.tick();
}