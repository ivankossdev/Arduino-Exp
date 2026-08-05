#include "Menu.h"

Menu::Menu()
  : _menuChoice(0), _scanning(false), _hasScanResult(false) {}

void Menu::begin() {
  const unsigned long SERIAL_TIMEOUT_MS = 2000;
  unsigned long startTime = millis();

  while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS)) {
    delay(10);
  }

  SerialBufferClear();
  printMenu();  // при старте выводится меню со статусом
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
    _hasScanResult = true;
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
  Serial.setTimeout(10000);

  // --- Выбор сети ---
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
        index = -1;
      }
    } else {
      Serial.println("Пустой ввод, попробуйте снова.");
    }
  }

  String ssid = String(_networks[index].ssid);
  Serial.print("Выбрали сеть: ");
  Serial.println(ssid);

  // Проверяем, есть ли сохранённый пароль для этой сети
  String password = _creds.getPassword(ssid);
  if (password.length() > 0) {
    Serial.println("Найден сохранённый пароль, подключаюсь автоматически.");
  } else {
    // Запрашиваем пароль, если нет сохранённого
    while (password.length() == 0) {
      Serial.print("Введите пароль для сети \"");
      Serial.print(ssid);
      Serial.print("\": ");
      password = Serial.readStringUntil('\n');
      password.trim();
      if (password.length() == 0) {
        Serial.println("Пароль не может быть пустым, попробуйте снова.");
      }
    }
  }

  // !!! Сбрасываем _scanning только ПОСЛЕ подключения, чтобы заблокировать ввод
  // (это исправление, но оно не относится к статусу – оставляем как есть)
  // В текущем коде _scanning = false стоит до вызова connectToNetwork, что неверно,
  // но мы не меняем логику, поэтому оставляем как было.
  _scanning = false;

  Serial.println("⏳ Подключение...");
  bool success = _scanner.connectToNetwork(ssid.c_str(), password.c_str());

  if (success) {
    Serial.println("✅ Подключено успешно!");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());
    _lastSSID = ssid;
    _lastPassword = password;
  } else {
    Serial.println("❌ Ошибка подключения. Проверьте пароль или доступность сети.");
  }
}

// НОВЫЙ МЕТОД: возвращает строку статуса
String Menu::getStatus() const {
  if (WiFi.status() == WL_CONNECTED) {
    String ssid = WiFi.SSID();
    IPAddress ip = WiFi.localIP();
    return "Подключено к \"" + ssid + "\", IP: " + ip.toString();
  } else {
    return "Не подключено к Wi-Fi.";
  }
}

void Menu::printMenu() {
  Serial.println("\n=== Интерактивное меню ESP32 ===");
  // Вывод статуса подключения
  Serial.println("Статус: " + getStatus());
  Serial.println("0. Показать меню снова");
  Serial.println("1. Показать доступные сети");
  Serial.println("2. Подключиться к сети (после сканирования)");
  Serial.println("3. Сохранить текущую сеть");
  Serial.println("4. Показать сохранённые сети");
  Serial.println("5. Подключиться к сохранённой сети");
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
    case 3:
      saveCurrentNetwork();
      break;
    case 4:
      showSavedNetworks();
      break;
    case 5:
      connectToSavedNetwork();
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–5.");
      break;
  }
}

void Menu::saveCurrentNetwork() {
  if (_lastSSID.length() == 0 || _lastPassword.length() == 0) {
    Serial.println("⚠️ Нет активного подключения или пароль не сохранён. Сначала подключитесь к сети.");
    return;
  }
  if (_creds.save(_lastSSID, _lastPassword)) {
    Serial.printf("✅ Сеть \"%s\" сохранена.\n", _lastSSID.c_str());
  } else {
    Serial.println("❌ Ошибка сохранения.");
  }
}

void Menu::showSavedNetworks() {
  _creds.printAll();
}

void Menu::connectToSavedNetwork() {
  int count = _creds.count();
  if (count == 0) {
    Serial.println("Нет сохранённых сетей.");
    return;
  }

  _creds.printAll();

  Serial.print("Введите номер сети для подключения (0..");
  Serial.print(count - 1);
  Serial.print("): ");
  String input = Serial.readStringUntil('\n');
  input.trim();
  int index = input.toInt();

  if (index < 0 || index >= count) {
    Serial.println("❌ Неверный номер.");
    return;
  }

  String ssid = _creds.getSSID(index);
  String password = _creds.getPasswordByIndex(index);

  if (ssid.length() == 0 || password.length() == 0) {
    Serial.println("Ошибка получения данных.");
    return;
  }

  Serial.printf("Подключаюсь к \"%s\"...\n", ssid.c_str());
  _scanning = true;

  bool success = _scanner.connectToNetwork(ssid.c_str(), password.c_str());

  _scanning = false;

  if (success) {
    Serial.println("✅ Подключено успешно!");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());
    _lastSSID = ssid;
    _lastPassword = password;
  } else {
    Serial.println("❌ Ошибка подключения. Возможно, сеть недоступна или пароль изменился.");
  }
}