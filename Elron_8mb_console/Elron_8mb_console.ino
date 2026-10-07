#include "EventTimer.h"
#include "Settings.h"
#include "Button.h"
#include "Console.h"

// ==================== КОНСТАНТЫ ====================
const uint8_t BTN_ACTIVE = HIGH;

// ==================== ОБЪЕКТЫ ====================
EventTimer blinkTimer(1000, 10);
Button button(BTN_BUILTIN, BTN_ACTIVE, 50);
Console console;

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

// ==================== ПРИМЕНЕНИЕ НАСТРОЕК ====================

// Единая точка «применить settings к железу».
// Вызывается:
//   - из setup() после settingsInit()
//   - из switchMode() и resetMode() после изменения settings.mode
//   - из onSettingsChanged() после команд Console
void applySettings() {
  blinkTimer.setMaxIterations(settings.blinkCount);
  button.setLongPressMs(settings.longPressMs);

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

// ==================== ЛОГИКА РЕЖИМОВ ====================

void switchMode() {
  switch (settings.mode) {
    case SLOW: settings.mode = FAST; break;
    case FAST: settings.mode = OFF; break;
    case OFF: settings.mode = SLOW; break;
  }

  settingsSave();
  applySettings();

  Serial.print("Mode -> ");
  Serial.println(settings.mode == SLOW ? "SLOW" : settings.mode == FAST ? "FAST"
                                                                        : "OFF");
}

void resetMode() {
  settings.mode = SLOW;
  applySettings();
  blinkTimer.reset();  // count = 0, state = false
  digitalWrite(LED_BUILTIN, LOW);
  settingsSave();

  Serial.println("Mode -> RESET");
}

// Колбэк для Console: настройки изменены — надо применить к железу
void onSettingsChanged() {
  applySettings();
}

// ==================== SETUP / LOOP ====================

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BTN_BUILTIN, INPUT);

  settingsInit();  // EEPROM.begin() + load + validate
  settingsIncBootCount();
  Serial.print("Boot #");
  Serial.println(settings.bootCount);

  delay(50);
  Serial.print("BTN idle reading: ");
  Serial.println(digitalRead(BTN_BUILTIN) ? "HIGH" : "LOW");

  // Таймер
  blinkTimer.setCallback(onBlink);
  blinkTimer.setSeriesCompleteCallback(onSeriesComplete);

  // Кнопка
  button.setPressStartCallback(settingsIncButtonCount);
  button.setShortPressCallback(switchMode);
  button.setLongPressCallback(resetMode);

  // Консоль
  console.setOnSettingsChanged(onSettingsChanged);

  // Применяем восстановленный режим из EEPROM
  applySettings();

  Serial.print("Mode -> ");
  Serial.println(settings.mode == SLOW ? "SLOW" : settings.mode == FAST ? "FAST"
                                                                        : "OFF");

  console.printPrompt();
}

void loop() {
  blinkTimer.tick();
  button.tick();
  console.tick();
}