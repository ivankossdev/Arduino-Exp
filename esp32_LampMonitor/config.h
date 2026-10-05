// ============================================================
// config.h — единое место для всех констант проекта
// ============================================================
// НОВЫЙ ФАЙЛ. Собран из «магических чисел», разбросанных
// по коду: таймауты, задержки, размеры, дефолтные значения,
// имена namespace-ов Preferences и т.п.
// ============================================================

#ifndef CONFIG_H
#define CONFIG_H

// ------------------------------------------------------------
// Serial / запуск
// ------------------------------------------------------------
#define SERIAL_BAUD              115200
#define STARTUP_DELAY_MS         100     // пауза после Serial.begin()

// ------------------------------------------------------------
// Дисплей (TTGO T-Display, альбомная ориентация)
// ------------------------------------------------------------
#define SCREEN_WIDTH             240
#define SCREEN_HEIGHT            135
#define DISPLAY_UPDATE_INTERVAL_MS  2000  // период обновления в update()
#define RSSI_QUANTIZE_STEP       5       // квантование RSSI (было /5*5)

// ------------------------------------------------------------
// Wi-Fi
// ------------------------------------------------------------
#define WIFI_SCAN_MAX_NETWORKS   50      // размер массива NetworkInfo
#define WIFI_CONNECT_TIMEOUT_MS  20000   // таймаут WiFi.begin()
#define WIFI_SETTLE_DELAY_MS     200     // пауза после disconnect/mode/шаг опроса
#define WIFI_AUTOCONNECT_RETRY_DELAY_MS  500  // пауза между попытками в autoConnect
#define WIFI_BOOT_WAIT_MS        10000   // сколько ждём WiFi при старте
#define WIFI_BOOT_POLL_MS        200     // шаг опроса при старте

// ------------------------------------------------------------
// AP-режим
// ------------------------------------------------------------
#define AP_SSID                  "ESP32-Setup"
#define AP_PASSWORD              "12345678"
#define WEB_AP_FALLBACK_TIMEOUT_MS   60000   // потеря Wi-Fi → перезагрузка

// ------------------------------------------------------------
// QR-код в AP-режиме
// ------------------------------------------------------------
// AP_QR_VERSION — версия QR (1..40). 3 = 29x29 модулей,
//                 хватает для Wi-Fi credentials до ~53 байт.
// AP_QR_ECC     — уровень коррекции ошибок: 0=L, 1=M, 2=Q, 3=H.
// AP_QR_SCALE   — размер модуля QR в пикселях (4 = 116x116 px
//                 для версии 3).
// AP_QR_MARGIN  — отступ QR от правого края экрана (px).
#define AP_QR_VERSION   3
#define AP_QR_ECC       0
#define AP_QR_SCALE     4
#define AP_QR_MARGIN    4

// === НОВОЕ: размер буфера для QR-кода. ===
// В библиотеке ricmoo qrcode_getBufferSize() — это функция,
// а не constexpr, поэтому в C++ её нельзя использовать как размер
// массива. Считаем размер по той же формуле, что и библиотека:
//     ((16 * version + 128) * version + 64) / 8 байт
// Для version=3 это 74 байта. Формула вычисляется препроцессором,
// результат — целочисленная константа, пригодная для массива.
#define AP_QR_BUFFER_SIZE  (((16 * AP_QR_VERSION + 128) * AP_QR_VERSION + 64) / 8)


// ------------------------------------------------------------
// MQTT
// ------------------------------------------------------------
#define MQTT_DEFAULT_PORT        1883
#define MQTT_DEFAULT_STATE_TOPIC "home/lamp/status"
#define MQTT_BUFFER_SIZE         1024
#define MQTT_RECONNECT_BASE_DELAY_MS  1000   // стартовая задержка backoff
#define MQTT_RECONNECT_MAX_DELAY_MS   60000  // потолок backoff
#define MQTT_DISCONNECT_DELAY_MS  50
#define MQTT_CLIENT_ID_PREFIX    "ESP32_Client_"

// ------------------------------------------------------------
// Веб-сервер
// ------------------------------------------------------------
#define WEB_SERVER_PORT          80
#define WEB_SCAN_DELAY_MS        100     // пауза после startScan() в handleScan
#define WEB_RESTART_DELAY_MS     100     // пауза перед ESP.restart()

// ------------------------------------------------------------
// JSON-документы (ArduinoJson)
// ------------------------------------------------------------
#define JSON_DOC_CONNECT_SIZE    256     // /connect
#define JSON_DOC_DELETE_SIZE     128     // /delete
#define JSON_DOC_MQTT_SIZE       512     // /mqtt POST

// ------------------------------------------------------------
// Хранилище Wi-Fi credentials (Preferences + JSON)
// ------------------------------------------------------------
#define WIFI_CRED_JSON_SIZE            2048
#define WIFI_CRED_MIGRATION_JSON_SIZE  1024

// ------------------------------------------------------------
// Preferences — namespace и ключи
// ------------------------------------------------------------
#define PREF_NAMESPACE_WIFI      "wifi"
#define PREF_KEY_WIFI            "creds"
#define PREF_NAMESPACE_MQTT      "mqtt"
#define PREF_KEY_MQTT            "config"

// ------------------------------------------------------------
// Логирование по категориям
// ------------------------------------------------------------
// Каждая категория — отдельный бит. Управление через маску
// LOG_ENABLED_CATEGORIES: отключённая категория полностью исчезает
// из прошивки (макрос LOG раскрывается в if с константным условием,
// оптимизатор выкидывает тело).
//
// Примеры настройки:
//   Всё включено:                     (LOG_CAT_ALL)
//   Всё кроме Wi-Fi и веба:           (LOG_CAT_ALL & ~LOG_CAT_WIFI & ~LOG_CAT_WEB)
//   Только ошибки и Wi-Fi:            (LOG_CAT_SYS | LOG_CAT_WIFI)
//   Логи полностью выключены:         0
//
// После правки — просто пересобрать. Никаких других файлов
// трогать не нужно.

#define LOG_CAT_NONE  0x0000
#define LOG_CAT_SYS   0x0001   // запуск, общие сообщения, ошибки инициализации
#define LOG_CAT_WIFI  0x0002   // Wi-Fi: сканирование, подключение, сохранение
#define LOG_CAT_MQTT  0x0004   // MQTT: соединение, реконнект, подписка
#define LOG_CAT_TFT   0x0008   // дисплей
#define LOG_CAT_LAMP  0x0010   // состояние лампы
#define LOG_CAT_WEB   0x0020   // веб-сервер
#define LOG_CAT_ALL   0xFFFF

// Что логировать. По умолчанию — всё, как было раньше.
#define LOG_ENABLED_CATEGORIES  (LOG_CAT_ALL)

// ------------------------------------------------------------
// Кнопки
// ------------------------------------------------------------
// Правая кнопка на TTGO T-Display v1.1 — GPIO35, input-only.
// Нажатие = LOW. На плате стоит внешний pull-up, поэтому
// pinMode(INPUT) без INPUT_PULLUP.
#define BUTTON_SERVICE_PIN        35
#define BUTTON_DEBOUNCE_MS        40

// ------------------------------------------------------------
// Сервисный экран (по кнопке)
// ------------------------------------------------------------
// Сколько держать сервисный экран после нажатия кнопки.
// По истечении — автоматический возврат к Lamp view.
#define SERVICE_VIEW_TIMEOUT_MS   5000

#endif