#include "EventTimer.h"
#include "Settings.h"
#include "Button.h"
#include "Console.h"

// ==================== КОНСТАНТЫ ====================
const uint8_t BTN_ACTIVE = HIGH;
const uint8_t LED2_PIN   = 4;    // Новый светодиод

// ==================== ОБЪЕКТЫ ====================
EventTimer blinkTimer(1000, 10);
EventTimer led2Timer(500, 1); 
Button button(BTN_BUILTIN, BTN_ACTIVE, 50);
Console console;

// ==================== КОЛБЭКИ ТАЙМЕРА ====================

void onBlink(bool state, int count) {
  digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
  Serial.print(settings.mode == SLOW ? "SLOW " : "FAST ");
  Serial.print("Count ");
  Serial.println(count);
}

void onLed2Blink(bool state, int count) {
  (void)count;
  digitalWrite(LED2_PIN, state ? HIGH : LOW);
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

  // LED1 — основная логика режимов
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

  // LED2 — независимый мигальщик
  if (settings.led2Interval == 0) {
    led2Timer.stop();
    digitalWrite(LED2_PIN, LOW);
  } else {
    led2Timer.setInterval(settings.led2Interval);
    led2Timer.start();
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
  pinMode(LED2_PIN, OUTPUT);

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
  led2Timer.setCallback(onLed2Blink);

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
  led2Timer.tick(); 
  button.tick();
  console.tick();
}