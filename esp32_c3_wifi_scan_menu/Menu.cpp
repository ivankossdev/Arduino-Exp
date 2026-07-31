#include "Menu.h"


Menu::Menu()
  : menuChoice(0) {}


void Menu::begin() {

  const unsigned long SERIAL_TIMEOUT_MS = 2000;
  unsigned long startTime = millis();

  while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS)) {
    delay(10);
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

void Menu::wifiScan(){
  scanner.scan(); 
}


/* Меню для пользователя  */
void Menu::printMenu() {
  Serial.println("\n=== Интерактивное меню ESP32 ===");
  Serial.println("0. Показать меню снова");
  Serial.println("1. Показать доступные сети");
}


void Menu::handleMenuChoice(int choice) {

  Serial.print("Выбрали: ");
  Serial.println(menuChoice);

  switch (choice) {
    case 0:
      printMenu();
      break;
    case 1: 
      wifiScan(); 
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–5.");
      break;
  }
}

