#include "Menu.h"
#include <cctype>   // для isdigit

// ============================================================
// Вспомогательная функция для проверки, что строка является числом
// ============================================================
static bool isNumber(const String& str) {
    if (str.length() == 0) return false;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        if (!isdigit(c) && !(i == 0 && c == '-')) {
            return false;
        }
    }
    return true;
}

Menu::Menu()
  : _menuChoice(0), _scanning(false), _hasScanResult(false), _networkCount(0) {}

// ============================================================
// Автоматическое подключение к первой сохранённой сети
// ============================================================
void Menu::autoConnect() {
  int count = _creds.count();
  if (count == 0) {
    Serial.println("ℹ️ Нет сохранённых сетей для автоподключения.");
    return;
  }

  String ssid = _creds.getSSID(0);
  String password = _creds.getPasswordByIndex(0);

  if (ssid.length() == 0 || password.length() == 0) {
    Serial.println("⚠️ Ошибка получения данных первой сети.");
    return;
  }

  Serial.printf("🔄 Автоподключение к \"%s\"...\n", ssid.c_str());

  _scanning = true;

  bool success = _scanner.connectToNetwork(ssid.c_str(), password.c_str());

  _scanning = false;

  if (success) {
    Serial.println("✅ Автоподключение успешно!");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());
    _lastSSID = ssid;
    _lastPassword = password;
  } else {
    Serial.println("❌ Автоподключение не удалось. Используйте меню для подключения вручную.");
  }
}

void Menu::begin() {
  const unsigned long SERIAL_TIMEOUT_MS = 2000;
  unsigned long startTime = millis();

  while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS)) {
    delay(10);
  }

  autoConnect();

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

  _networkCount = (count > 0) ? count : 0;

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

  if (_networkCount == 0) {
    Serial.println("⚠️ Нет доступных сетей для подключения.");
    return;
  }

  _scanning = true;
  unsigned long oldTimeout = Serial.getTimeout();
  Serial.setTimeout(10000);

  int index = -1;
  while (index < 0 || index >= _networkCount) {
    Serial.printf("Введите номер сети (0..%d): ", _networkCount - 1);
    String indexStr = Serial.readStringUntil('\n');
    indexStr.trim();
    if (indexStr.length() == 0) {
      Serial.println("Пустой ввод, попробуйте снова.");
      continue;
    }
    if (!isNumber(indexStr)) {
      Serial.println("❌ Введите число!");
      continue;
    }
    index = indexStr.toInt();
    if (index < 0 || index >= _networkCount) {
      Serial.println("❌ Неверный номер, попробуйте снова.");
      index = -1;
    }
  }

  String ssid = String(_networks[index].ssid);
  Serial.print("Выбрали сеть: ");
  Serial.println(ssid);

  String password = _creds.getPassword(ssid);
  if (password.length() > 0) {
    Serial.println("Найден сохранённый пароль, подключаюсь автоматически.");
  } else {
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

  Serial.setTimeout(oldTimeout);

  Serial.println("⏳ Подключение...");
  bool success = _scanner.connectToNetwork(ssid.c_str(), password.c_str());

  _scanning = false;

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
  Serial.println("Статус: " + getStatus());
  Serial.println("0. Показать меню снова");
  Serial.println("1. Показать доступные сети");
  Serial.println("2. Подключиться к сети (после сканирования)");
  Serial.println("3. Сохранить текущую сеть");
  Serial.println("4. Показать сохранённые сети");
  Serial.println("5. Подключиться к сохранённой сети");
  Serial.println("6. Удалить сохранённую сеть");
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
    case 6:
      deleteSavedNetwork();
      break;
    default:
      Serial.println("Неверная опция! Выберите 0–6.");
      break;
  }
}

// ============================================================
// Удаление сохранённой сети (исправлено: не обнуляем _lastSSID/_lastPassword)
// ============================================================
void Menu::deleteSavedNetwork() {
  int count = _creds.count();
  if (count == 0) {
    Serial.println("Нет сохранённых сетей для удаления.");
    return;
  }

  _creds.printAll();

  SerialBufferClear();
  unsigned long oldTimeout = Serial.getTimeout();
  Serial.setTimeout(10000);

  int index = -1;
  while (index < 0 || index >= count) {
    Serial.print("Введите номер сети для удаления (0..");
    Serial.print(count - 1);
    Serial.print("): ");
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) {
      Serial.println("Пустой ввод, попробуйте снова.");
      continue;
    }
    if (!isNumber(input)) {
      Serial.println("❌ Введите число!");
      continue;
    }
    index = input.toInt();
    if (index < 0 || index >= count) {
      Serial.println("❌ Неверный номер, попробуйте снова.");
      index = -1;
    }
  }

  Serial.setTimeout(oldTimeout);

  String ssid = _creds.getSSID(index);
  if (ssid.length() == 0) {
    Serial.println("Ошибка получения SSID.");
    return;
  }

  Serial.print("Вы уверены, что хотите удалить сеть \"");
  Serial.print(ssid);
  Serial.print("\"? (y/n): ");

  Serial.setTimeout(5000);
  String confirm = Serial.readStringUntil('\n');
  confirm.trim();
  Serial.setTimeout(oldTimeout);

  if (confirm.equalsIgnoreCase("y") || confirm.equalsIgnoreCase("yes")) {
    if (_creds.remove(ssid)) {
      Serial.printf("✅ Сеть \"%s\" удалена.\n", ssid.c_str());
      // *** ИСПРАВЛЕНИЕ: НЕ ОБНУЛЯЕМ _lastSSID и _lastPassword ***
      // if (_lastSSID == ssid) {
      //   _lastSSID = "";
      //   _lastPassword = "";
      // }
      // Теперь, даже если удалили текущую сеть, переменные остаются,
      // и пункт "Сохранить текущую сеть" продолжает работать.
    } else {
      Serial.println("❌ Ошибка удаления.");
    }
  } else {
    Serial.println("❌ Удаление отменено.");
  }
}

// ============================================================
// Сохранение текущей сети (улучшено: проверка фактического подключения)
// ============================================================
void Menu::saveCurrentNetwork() {
  // Проверяем фактическое подключение
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ Нет подключения к Wi-Fi.");
    return;
  }

  String currentSSID = WiFi.SSID();
  if (currentSSID.length() == 0) {
    Serial.println("⚠️ Не удалось получить имя текущей сети.");
    return;
  }

  // Если _lastSSID не совпадает с текущим или пуст, обновляем из WiFi
  if (_lastSSID != currentSSID) {
    Serial.printf("Обновляем кеш: текущая сеть \"%s\"\n", currentSSID.c_str());
    _lastSSID = currentSSID;
    // Пароль мы не можем получить из WiFi, но если он был сохранён ранее,
    // можно попытаться найти в хранилище. Если нет - запросим.
    String savedPass = _creds.getPassword(currentSSID);
    if (savedPass.length() > 0) {
      _lastPassword = savedPass;
      Serial.println("Найден сохранённый пароль, используем его.");
    } else {
      // Если пароль не сохранён, запросим у пользователя (иначе не сможем сохранить)
      Serial.println("Пароль для этой сети не найден в хранилище.");
      Serial.print("Введите пароль для сети \"");
      Serial.print(currentSSID);
      Serial.print("\": ");
      String pass = Serial.readStringUntil('\n');
      pass.trim();
      if (pass.length() == 0) {
        Serial.println("❌ Пароль не может быть пустым, сохранение отменено.");
        return;
      }
      _lastPassword = pass;
    }
  }

  // Теперь у нас есть _lastSSID и _lastPassword (либо обновлённые, либо старые)
  if (_lastSSID.length() == 0 || _lastPassword.length() == 0) {
    Serial.println("⚠️ Ошибка: нет данных для сохранения.");
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

  // *** ИСПРАВЛЕНИЕ: очистка буфера перед запросом ***
  SerialBufferClear();

  unsigned long oldTimeout = Serial.getTimeout();

  // *** ИСПРАВЛЕНИЕ: установка таймаута 10 секунд ***
  Serial.setTimeout(10000);

  Serial.print("Введите номер сети для подключения (0..");
  Serial.print(count - 1);
  Serial.print("): ");
  String input = Serial.readStringUntil('\n');
  input.trim();

  // *** ИСПРАВЛЕНИЕ: восстановление старого таймаута ***
  Serial.setTimeout(oldTimeout);

  if (input.length() == 0) {
    Serial.println("Пустой ввод, отмена.");
    return;
  }
  if (!isNumber(input)) {
    Serial.println("❌ Введите число!");
    return;
  }
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