#include "Menu.h"

Menu::Menu()
  : _menuChoice(0), _scanning(false) {}

void Menu::begin() {
  const unsigned long SERIAL_TIMEOUT_MS = 2000;
  unsigned long startTime = millis();

  while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS)) {
    delay(10);
  }

  SerialBufferClear();
  printMenu();
}

void Menu::update() {
  // Если сканирование выполняется, не принимаем новые команды
  if (_scanning) {
    return;
  }

  if (Serial.available() > 0) {
    // Читаем строку до символа новой строки
    String input = Serial.readStringUntil('\n');
    input.trim();                     // Удаляем пробелы и \r
    if (input.length() == 0) {
      return;                         // Пустой ввод игнорируем
    }

    // Преобразуем в число
    _menuChoice = input.toInt();

    // Обрабатываем команду
    handleMenuChoice(_menuChoice);

    // Напоминаем, как вызвать меню (если команда не 0)
    if (_menuChoice != 0) {
      Serial.println("\nПоказать меню: 0");
    }

    SerialBufferClear(); // Очищаем буфер на случай лишних символов
  }
}

/* Очистка буфера COM порта */
void Menu::SerialBufferClear() {
  while (Serial.available()) {
    Serial.read();
  }
}

/* Запуск сканирования с блокировкой ввода */
void Menu::wifiScan() {
  _scanning = true;                     // Блокируем ввод

  // Выполняем сканирование, заполняем _networks
  int count = _scanner.scan(_networks, MAX_NETWORKS);

  if (count > 0) {
    displayNetworks(_networks, count);
  } else if (count == 0) {
    Serial.println("❌ Сетей не найдено.");
  } else {
    Serial.println("❌ Ошибка сканирования.");
  }

  _scanning = false;                    // Снимаем блокировку
}

/* Вывод таблицы с сетями (перенесено из WiFiScanner) */
void Menu::displayNetworks(const NetworkInfo* networks, int count) {
  if (count <= 0) return;

  Serial.printf("✅ Найдено %d сетей:\n\n", count);

  // Заголовки
  const char* headers[] = {"#", "CH", "RSSI", "ENC", "HID", "BSSID", "SSID"};
  const int colCount = 7;

  // Вычисляем ширину колонок
  int widths[colCount];
  for (int i = 0; i < colCount; i++) {
    widths[i] = strlen(headers[i]);
  }

  for (int i = 0; i < count; i++) {
    char numStr[8];   snprintf(numStr, sizeof(numStr), "%d", i);
    char chStr[8];    snprintf(chStr, sizeof(chStr), "%d", networks[i].channel);
    char rssiStr[8];  snprintf(rssiStr, sizeof(rssiStr), "%d", networks[i].rssi);
    const char* encStr = _scanner.getEncryptionType(networks[i].encryptionType);
    const char* hiddenStr = networks[i].hidden ? "Yes" : "No";

    if (strlen(numStr) > widths[0]) widths[0] = strlen(numStr);
    if (strlen(chStr) > widths[1]) widths[1] = strlen(chStr);
    if (strlen(rssiStr) > widths[2]) widths[2] = strlen(rssiStr);
    if (strlen(encStr) > widths[3]) widths[3] = strlen(encStr);
    if (strlen(hiddenStr) > widths[4]) widths[4] = strlen(hiddenStr);
    if (strlen(networks[i].bssid) > widths[5]) widths[5] = strlen(networks[i].bssid);
    if (strlen(networks[i].ssid) > widths[6]) widths[6] = strlen(networks[i].ssid);
  }

  // Добавляем отступ между колонками
  for (int i = 0; i < colCount; i++) widths[i] += 1;

  // Вывод заголовков
  for (int i = 0; i < colCount; i++) {
    Serial.printf("%-*s", widths[i], headers[i]);
    if (i < colCount - 1) Serial.print(" | ");
  }
  Serial.println();

  // Разделительная линия
  for (int i = 0; i < colCount; i++) {
    for (int j = 0; j < widths[i]; j++) Serial.print("-");
    if (i < colCount - 1) Serial.print("-+-");
  }
  Serial.println();

  // Вывод данных
  for (int i = 0; i < count; i++) {
    char numStr[8];   snprintf(numStr, sizeof(numStr), "%d", i);
    char chStr[8];    snprintf(chStr, sizeof(chStr), "%d", networks[i].channel);
    char rssiStr[8];  snprintf(rssiStr, sizeof(rssiStr), "%d", networks[i].rssi);
    const char* encStr = _scanner.getEncryptionType(networks[i].encryptionType);
    const char* hiddenStr = networks[i].hidden ? "Yes" : "No";

    Serial.printf("%-*s", widths[0], numStr);
    Serial.print(" | ");
    Serial.printf("%-*s", widths[1], chStr);
    Serial.print(" | ");
    Serial.printf("%-*s", widths[2], rssiStr);
    Serial.print(" | ");
    Serial.printf("%-*s", widths[3], encStr);
    Serial.print(" | ");
    Serial.printf("%-*s", widths[4], hiddenStr);
    Serial.print(" | ");
    Serial.printf("%-*s", widths[5], networks[i].bssid);
    Serial.print(" | ");
    Serial.printf("%-*s", widths[6], networks[i].ssid);
    Serial.println();
  }
  Serial.println();
}

/* Меню для пользователя */
void Menu::printMenu() {
  Serial.println("\n=== Интерактивное меню ESP32 ===");
  Serial.println("0. Показать меню снова");
  Serial.println("1. Показать доступные сети");
}

void Menu::handleMenuChoice(int choice) {
  Serial.print("Выбрали: ");
  Serial.println(choice);

  switch (choice) {
    case 0:
      printMenu();
      break;
    case 1:
      wifiScan();
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–1.");
      break;
  }
}