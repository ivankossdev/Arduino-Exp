#include "Menu.h"


Menu::Menu(int pin)
  : ledPin(pin), menuChoice(0) {}

void Menu::printSystemStatus() {
  Serial.println("\n=== СТАТУС СИСТЕМЫ ===");

  // Свободная RAM
  Serial.print("Свободная RAM: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" байт");

  // Размер Flash
  printFlashInfo();

  // Остаток стека
  unsigned long stackFree = uxTaskGetStackHighWaterMark(NULL);
  Serial.printf("Остаток стека: %lu байт\n", stackFree);

  Serial.println("====================\n");
}


void Menu::printFlashInfo() {
  Serial.printf("Размер Flash: %lu МБ\n",
    ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("Скорость Flash: %lu МГц\n", ESP.getFlashChipSpeed() / 1000000);
}


void Menu::begin() {
  Serial.begin(115200);

  const unsigned long SERIAL_TIMEOUT_MS = 2000;
  unsigned long startTime = millis();

  // 
  while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS)) {
    delay(10);
  }

  if (Serial) {
    Serial.println("ESP32-C3 Super Mini: старт инициализации...");
  }

  printSystemStatus();

  if (!led.begin(ledPin)) {
    if (Serial) {
      Serial.println("[ОШИБКА] Некорректный пин для светодиода");
    }
    while (true);
  }

  led.blink(); // Сигнал старта

  preferences.begin("menu", false);
  loadSettings();
  applyLedState();

  if (Serial) {
    Serial.println("Инициализация завершена. Готов к работе!");
  }
  SerialBuferClear();
  printMenu();
}


void Menu::update() {
  if (Serial.available() > 0) {

    menuChoice = Serial.parseInt();
    handleMenuChoice(menuChoice);

    if (menuChoice != 0) {
      Serial.println("\nПоказать меню: 0");
    }

    SerialBuferClear();  // Очищаем буфер порта от мусора
  }
}


/* Очистка буфера COM порта */
void Menu::SerialBuferClear() {
  while (Serial.available()) {
    Serial.read();
  }
}


/* Меню для пользователя  */
void Menu::printMenu() {
  Serial.println("\n=== Интерактивное меню ESP32 ===");
  Serial.println("1. Включить светодиод");
  Serial.println("2. Выключить светодиод");
  Serial.println("3. Показать состояние");
  Serial.println("4. Сохранить настройки");
  Serial.println("5. Сброс на заводсике настройки");
  Serial.println("0. Показать меню снова");
}


void Menu::handleMenuChoice(int choice) {

  Serial.print("Выбрали: ");
  Serial.println(menuChoice);

  switch (choice) {
    case 1:
      led.on();
      Serial.println("Светодиод ВКЛЮЧЕН");
      break;
    case 2:
      led.off();
      Serial.println("Светодиод ВЫКЛЮЧЕН");
      break;
    case 3:
      Serial.print("Состояние светодиода: ");
      Serial.println(led.getState() ? "ВКЛЮЧЕН" : "ВЫКЛЮЧЕН");
      break;
    case 4:
      saveSettings();
      Serial.println("Настройки сохранены");
      break;
    case 5:
      led.setState(false);
      saveSettings();
      applyLedState();
      Serial.println("Настройки сброшены");
      break;
    case 0:
      printMenu();
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–5.");
      break;
  }
}


void Menu::saveSettings() {
  preferences.putBool("led_state", led.getState());
}


void Menu::loadSettings() {
  led.setState(preferences.getBool("led_state", false));
}


void Menu::applyLedState() {
  if (led.getState()) {
    led.on();
  } else {
    led.off();
  }
}
