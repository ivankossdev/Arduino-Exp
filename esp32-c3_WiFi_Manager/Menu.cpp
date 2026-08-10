#include "Menu.h"
#include <cctype>

// Вспомогательная проверка числа
static bool isNumber(const String& str) {
    if (str.length() == 0) return false;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        if (!isdigit(c) && !(i == 0 && c == '-')) return false;
    }
    return true;
}

Menu::Menu(AppState& appState) : _menuChoice(0), _appState(appState) {}

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
    AppStateEnum state = _appState.getState();
    if (state == AppStateEnum::SCANNING || state == AppStateEnum::CONNECTING) {
        return; // блокируем ввод
    }

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

void Menu::printMenu() {
    Serial.println("\n=== Интерактивное меню ESP32 ===");
    Serial.println("Статус: " + _appState.getStatusString());
    Serial.println("0. Показать меню снова");
    Serial.println("1. Показать доступные сети");
    Serial.println("2. Подключиться к сети (после сканирования)");
    Serial.println("3. Сохранить текущую сеть");
    Serial.println("4. Показать сохранённые сети");
    Serial.println("5. Подключиться к сохранённой сети");
    Serial.println("6. Удалить сохранённую сеть");
    Serial.println("7. Настройки MQTT");
}

void Menu::handleMenuChoice(int choice) {
    Serial.print("Выбрали: ");
    Serial.println(choice);

    switch (choice) {
        case 0:
            printMenu();
            break;
        case 1: {
            bool started = _appState.startScan();
            if (!started) {
                Serial.println("❌ Ошибка сканирования или уже выполняется.");
            } else {
                displayNetworksFromState();
            }
            break;
        }
        case 2:
            connectToNetwork();
            break;
        case 3:
            _appState.saveCurrentNetwork();
            break;
        case 4:
            _appState.printSavedNetworks();
            break;
        case 5:
            connectToSavedNetwork();
            break;
        case 6:
            deleteSavedNetwork();
            break;
        case 7:
            displayMqttSettings();
            break;
        default:
            Serial.println("Неверная опция! Выберите 0–7.");
            break;
    }
}

void Menu::displayNetworksFromState() {
    int count = _appState.getNetworkCount();
    if (count <= 0) {
        Serial.println("❌ Нет сетей для отображения.");
        return;
    }

    Serial.printf("✅ Найдено %d сетей:\n\n", count);

    const char* headers[] = { "#", "CH", "RSSI", "ENC", "HID", "BSSID", "SSID" };
    const int colCount = 7;
    int widths[colCount];
    for (int i = 0; i < colCount; i++) {
        widths[i] = strlen(headers[i]);
    }

    for (int i = 0; i < count; i++) {
        NetworkInfo info = _appState.getNetwork(i);
        char numStr[8]; snprintf(numStr, sizeof(numStr), "%d", i);
        char chStr[8]; snprintf(chStr, sizeof(chStr), "%d", info.channel);
        char rssiStr[8]; snprintf(rssiStr, sizeof(rssiStr), "%d", info.rssi);
        const char* encStr = _appState.getEncryptionType(info.encryptionType);
        const char* hiddenStr = info.hidden ? "Yes" : "No";

        if (strlen(numStr) > widths[0]) widths[0] = strlen(numStr);
        if (strlen(chStr) > widths[1]) widths[1] = strlen(chStr);
        if (strlen(rssiStr) > widths[2]) widths[2] = strlen(rssiStr);
        if (strlen(encStr) > widths[3]) widths[3] = strlen(encStr);
        if (strlen(hiddenStr) > widths[4]) widths[4] = strlen(hiddenStr);
        if (strlen(info.bssid) > widths[5]) widths[5] = strlen(info.bssid);
        if (strlen(info.ssid) > widths[6]) widths[6] = strlen(info.ssid);
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
        NetworkInfo info = _appState.getNetwork(i);
        char numStr[8]; snprintf(numStr, sizeof(numStr), "%d", i);
        char chStr[8]; snprintf(chStr, sizeof(chStr), "%d", info.channel);
        char rssiStr[8]; snprintf(rssiStr, sizeof(rssiStr), "%d", info.rssi);
        const char* encStr = _appState.getEncryptionType(info.encryptionType);
        const char* hiddenStr = info.hidden ? "Yes" : "No";

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
        Serial.printf("%-*s", widths[5], info.bssid);
        Serial.print(" | ");
        Serial.printf("%-*s", widths[6], info.ssid);
        Serial.println();
    }
    Serial.println();
}

void Menu::connectToNetwork() {
    int count = _appState.getNetworkCount();
    if (count == 0) {
        Serial.println("⚠️ Нет доступных сетей. Сначала выполните сканирование (команда 1).");
        return;
    }

    displayNetworksFromState();

    unsigned long oldTimeout = Serial.getTimeout();
    Serial.setTimeout(10000);

    int index = -1;
    while (index < 0 || index >= count) {
        Serial.printf("Введите номер сети (0..%d): ", count - 1);
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

    NetworkInfo info = _appState.getNetwork(index);
    String ssid = String(info.ssid);
    Serial.print("Выбрали сеть: ");
    Serial.println(ssid);

    String password;
    if (_appState.hasSavedPassword(ssid)) {
        password = _appState.getSavedPassword(ssid);
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

    _appState.connectToNetwork(ssid, password);
}

void Menu::connectToSavedNetwork() {
    int count = _appState.getSavedCount();
    if (count == 0) {
        Serial.println("Нет сохранённых сетей.");
        return;
    }

    _appState.printSavedNetworks();

    SerialBufferClear();
    unsigned long oldTimeout = Serial.getTimeout();
    Serial.setTimeout(10000);

    int index = -1;
    while (index < 0 || index >= count) {
        Serial.printf("Введите номер сети для подключения (0..%d): ", count - 1);
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
    _appState.connectToSavedNetwork(index);
}

void Menu::deleteSavedNetwork() {
    int count = _appState.getSavedCount();
    if (count == 0) {
        Serial.println("Нет сохранённых сетей для удаления.");
        return;
    }

    _appState.printSavedNetworks();

    SerialBufferClear();
    unsigned long oldTimeout = Serial.getTimeout();
    Serial.setTimeout(10000);

    int index = -1;
    while (index < 0 || index >= count) {
        Serial.printf("Введите номер сети для удаления (0..%d): ", count - 1);
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

    // Подтверждение удаления
    String ssid = _appState.getSavedSSID(index);
    Serial.print("Вы уверены, что хотите удалить сеть \"");
    Serial.print(ssid);
    Serial.print("\"? (y/n): ");

    Serial.setTimeout(5000);
    String confirm = Serial.readStringUntil('\n');
    confirm.trim();
    Serial.setTimeout(oldTimeout);

    if (confirm.equalsIgnoreCase("y") || confirm.equalsIgnoreCase("yes")) {
        if (_appState.deleteSavedNetwork(index)) {
            Serial.printf("✅ Сеть \"%s\" удалена.\n", ssid.c_str());
        } else {
            Serial.println("❌ Ошибка удаления.");
        }
    } else {
        Serial.println("❌ Удаление отменено.");
    }
}

// === Новые методы для MQTT ===

void Menu::displayMqttSettings() {
    // Загружаем последние сохранённые настройки (или текущие в памяти)
    _appState.loadMqttCredentials();
    MqttCredentials& creds = _appState.getMqttCredentials();

    Serial.println("\n=== Текущие настройки MQTT ===");
    Serial.printf("Сервер: %s\n", creds.getServer().c_str());
    Serial.printf("Порт: %d\n", creds.getPort());
    Serial.printf("Пользователь: %s\n", creds.getUser().c_str());
    Serial.printf("Пароль: %s\n", creds.getPassword().c_str());
    Serial.printf("Командный топик: %s\n", creds.getCmdTopic().c_str());
    Serial.printf("Топик состояния: %s\n", creds.getStateTopic().c_str());

    Serial.println("\nВведите 1 для изменения настроек, 0 для возврата в меню.");
    SerialBufferClear();
    unsigned long oldTimeout = Serial.getTimeout();
    Serial.setTimeout(10000);
    String input = Serial.readStringUntil('\n');
    input.trim();
    Serial.setTimeout(oldTimeout);

    if (input == "1") {
        editMqttSettings();
    } else {
        Serial.println("Возврат в меню.");
    }
}

void Menu::editMqttSettings() {
    Serial.println("\n=== Изменение настроек MQTT ===");
    Serial.println("Введите новые значения или оставьте строку пустой, чтобы сохранить текущие.");

    MqttCredentials& creds = _appState.getMqttCredentials();

    // Вспомогательная функция для ввода строки с сохранением старого значения
    auto promptString = [&](const char* prompt, String& target) {
        Serial.printf("%s [%s]: ", prompt, target.c_str());
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            target = input;
        }
    };

    // Вспомогательная функция для ввода порта
    auto promptPort = [&](const char* prompt, int& target) {
        Serial.printf("%s [%d]: ", prompt, target);
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0 && isNumber(input)) {
            target = input.toInt();
        }
    };

    // Устанавливаем таймаут для ввода
    unsigned long oldTimeout = Serial.getTimeout();
    Serial.setTimeout(30000); // даём больше времени на ввод всех параметров

    String server = creds.getServer();
    int port = creds.getPort();
    String user = creds.getUser();
    String password = creds.getPassword();
    String cmdTopic = creds.getCmdTopic();
    String stateTopic = creds.getStateTopic();

    promptString("Сервер", server);
    promptPort("Порт", port);
    promptString("Пользователь", user);
    promptString("Пароль", password);
    promptString("Командный топик", cmdTopic);
    promptString("Топик состояния", stateTopic);

    // Восстанавливаем таймаут
    Serial.setTimeout(oldTimeout);

    // Применяем изменения
    if (_appState.configureMqtt(server, port, user, password, cmdTopic, stateTopic)) {
        Serial.println("✅ Настройки MQTT сохранены.");
        // Перезапускаем MQTT с новыми настройками
        if (_appState.beginMqtt()) {
            Serial.println("✅ MQTT перезапущен с новыми настройками.");
        } else {
            Serial.println("⚠️ Ошибка запуска MQTT с новыми настройками.");
        }
    } else {
        Serial.println("❌ Ошибка сохранения настроек MQTT.");
    }
}