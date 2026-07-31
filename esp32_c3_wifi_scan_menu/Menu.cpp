#include "Menu.h"

Menu::Menu()
  : _menuChoice(0), _scanning(false), _hasScanResult(false), _terminalIsBusy(false) {}

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
  if (_scanning) return;

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) return;

    _menuChoice = input.toInt();
    handleMenuChoice(_menuChoice);

    if (_menuChoice != 0) {
      Serial.println("\nПоказать меню: 0");
    }
    SerialBufferClear();
  }
}

void Menu::SerialBufferClear() {
  while (Serial.available()) {
    Serial.read();
  }
}

void Menu::wifiScan() {
  _scanning = true;
  int count = _scanner.scan(_networks, MAX_NETWORKS);

  if (count > 0) {
    _hasScanResult = true;  // запоминаем, что сканирование прошло успешно
    displayNetworks(_networks, count);
  } else if (count == 0) {
    _hasScanResult = false;
    Serial.println("❌ Сетей не найдено.");
  } else {
    _hasScanResult = false;
    Serial.println("❌ Ошибка сканирования.");
  }

  _scanning = false;
}

void Menu::displayNetworks(const NetworkInfo* networks, int count) {
  if (count <= 0) return;

  Serial.printf("✅ Найдено %d сетей:\n\n", count);

  const char* headers[] = { "#", "CH", "RSSI", "ENC", "HID", "BSSID", "SSID" };
  const int colCount = 7;
  int widths[colCount];
  for (int i = 0; i < colCount; i++) {
    widths[i] = strlen(headers[i]);
  }

  for (int i = 0; i < count; i++) {
    char numStr[8];
    snprintf(numStr, sizeof(numStr), "%d", i);
    char chStr[8];
    snprintf(chStr, sizeof(chStr), "%d", networks[i].channel);
    char rssiStr[8];
    snprintf(rssiStr, sizeof(rssiStr), "%d", networks[i].rssi);
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

  for (int i = 0; i < colCount; i++) widths[i] += 1;

  for (int i = 0; i < colCount; i++) {
    Serial.printf("%-*s", widths[i], headers[i]);
    if (i < colCount - 1) Serial.print(" | ");
  }
  Serial.println();

  for (int i = 0; i < colCount; i++) {
    for (int j = 0; j < widths[i]; j++) Serial.print("-");
    if (i < colCount - 1) Serial.print("-+-");
  }
  Serial.println();

  for (int i = 0; i < count; i++) {
    char numStr[8];
    snprintf(numStr, sizeof(numStr), "%d", i);
    char chStr[8];
    snprintf(chStr, sizeof(chStr), "%d", networks[i].channel);
    char rssiStr[8];
    snprintf(rssiStr, sizeof(rssiStr), "%d", networks[i].rssi);
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

void Menu::connectToNetwork() {
  if (!_hasScanResult) {
    Serial.println("⚠️ Сначала выполните сканирование (команда 1).");
    return;
  }

  _scanning = true;

  // Устанавливаем таймаут на чтение Serial (по умолчанию 1000 мс, увеличим до 10 секунд)
  Serial.setTimeout(10000);

  // --- Запрос номера сети с проверкой на пустой ввод ---
  int index = -1;
  while (index < 0 || index >= MAX_NETWORKS || _networks[index].ssid[0] == '\0') {
    Serial.print("Введите номер сети (0..");
    Serial.print(MAX_NETWORKS - 1);
    Serial.print("): ");
    String indexStr = Serial.readStringUntil('\n');
    indexStr.trim();
    if (indexStr.length() > 0) {
      index = indexStr.toInt();
      if (index < 0 || index >= MAX_NETWORKS || _networks[index].ssid[0] == '\0') {
        Serial.println("❌ Неверный номер, попробуйте снова.");
        index = -1;  // сбросить для повторного запроса
      }
    } else {
      Serial.println("Пустой ввод, попробуйте снова.");
    }
  }

  Serial.print("Выбрали сеть: ");
  Serial.println(_networks[index].ssid);

  // --- Запрос пароля с проверкой на пустой ввод ---
  String password = "";
  while (password.length() == 0) {
    Serial.print("Введите пароль для сети \"");
    Serial.print(_networks[index].ssid);
    Serial.print("\": ");
    password = Serial.readStringUntil('\n');
    password.trim();
    if (password.length() == 0) {
      Serial.println("Пароль не может быть пустым, попробуйте снова.");
    }
  }

  _scanning = false;

  // --- Подключение ---
  Serial.println("⏳ Подключение...");
  bool success = _scanner.connectToNetwork(_networks[index].ssid, password.c_str());

  if (success) {
    Serial.println("✅ Подключено успешно!");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("❌ Ошибка подключения. Проверьте пароль или доступность сети.");
  }
}

void Menu::printMenu() {
  Serial.println("\n=== Интерактивное меню ESP32 ===");
  Serial.println("0. Показать меню снова");
  Serial.println("1. Показать доступные сети");
  Serial.println("2. Подключиться к сети (после сканирования)");  // новая команда
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
    case 2:
      connectToNetwork();
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–2.");
      break;
  }
}