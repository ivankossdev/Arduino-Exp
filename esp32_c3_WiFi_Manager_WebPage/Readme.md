# ESP32 Wi-Fi Manager с MQTT, OLED и веб-интерфейсом

Прошивка для ESP32-C3: управление Wi-Fi, MQTT, светодиодом и OLED-дисплеем через веб-интерфейс. Модульная архитектура с разделением ответственности по сервисам.

## Возможности

    Wi-Fi: сканирование сетей, подключение, хранение нескольких SSID/паролей, автоподключение при старте.

    Режим AP: если сохранённых сетей нет или автоподключение не удалось — устройство поднимает точку доступа для настройки.

    Веб-интерфейс:

        AP-режим: страница настройки (сканирование, подключение, сохранённые сети, MQTT).

        Клиентский режим: страница управления (статус, светодиод, MQTT).

    MQTT: подключение к брокеру, подписка на командный топик, публикация состояния, синхронизация с retained-значением при (пере)подключении.

    OLED-дисплей: отображение режима (AP/клиент), Wi-Fi, MQTT, состояния светодиода.

    Светодиод: локальное управление (веб) и удалённое (MQTT), с публикацией состояния на брокер.

## Архитектура

### Слои

```text
┌────────────────────────────────────────────────────────┐
│  esp32_c3_WiFi_Manager_WebPage.ino   (точка входа)     │
│      AppState, WebService                              │
└────────────────────────────────────────────────────────┘
                        │
        ┌───────────────┼───────────────┐
        ▼               ▼               ▼
   WebService     DisplayService    (AppState)
        │               │               │
        └───────┬───────┴───────┬───────┘
                ▼               ▼
          LedService       StatusBuilder
                │
                ▼
        ┌───────┴────────┬────────────┐
        ▼                ▼            ▼
   WiFiService      MqttService   StateManager
        │                │
        ├─ WiFiManager   ├─ MqttManager
        └─ WiFiCredentials └─ MqttCredentials
                              │
                              └─ LedManager (через LedService)

```

## Ключевые принципы

    AppState — композиционный корень. Владеет сервисами, роутит MQTT-сообщения и предоставляет API для WebService/DisplayService. Логики минимум.

    Сервисы (WiFiService, MqttService, LedService) — обёртки над низкоуровневыми менеджерами и хранилищами.

    Менеджеры (WiFiManager, MqttManager, LedManager) — низкоуровневая работа с железом/библиотеками.

    Хранилища (WiFiCredentials, MqttCredentials) — персистентность через Preferences в JSON.

    StateManager — единая машина состояний приложения, уведомляет подписчиков через колбэк.

## Структура проекта

```text
ESP32-C3_WiFi_Manager/
├── esp32_c3_WiFi_Manager_WebPage.ino   # setup() и loop()
│
├── AppState.h / .cpp                   # композиционный корень + MQTT-роутер
├── StateManager.h / .cpp               # машина состояний + колбэк на изменение
│
├── WiFiManager.h / .cpp                # низкоуровневые Wi-Fi операции
├── WiFiCredentials.h / .cpp            # хранение SSID/паролей
├── WiFiService.h / .cpp                # сервис Wi-Fi (фасад)
│
├── MqttManager.h / .cpp                # обёртка PubSubClient
├── MqttCredentials.h / .cpp            # хранение MQTT-настроек
├── MqttService.h / .cpp                # сервис MQTT (фасад)
│
├── LedManager.h / .cpp                 # низкоуровневое управление светодиодом
├── LedService.h / .cpp                 # сервис светодиода (MQTT-интеграция)
│
├── DisplayService.h / .cpp             # OLED-дисплей
│
├── WebService.h / .cpp                 # HTTP-сервер, страницы, REST-роуты
├── StatusBuilder.h / .cpp              # сборка JSON/текстового статуса
│
└── README.md
```

## Как это работает

### Запуск (setup)

    Инициализируется светодиод (AppState::beginLed(10, false)).

    Инициализируется OLED (DisplayService::begin).

    Проверяется наличие сохранённых Wi-Fi сетей:

        нет → сразу AP-режим (WebService::begin(true));

        есть → AppState::begin() запускает автоподключение, ждём до 10 с:

            успех → WebService::begin(false) (клиентский режим);

            неудача → AP-режим.

    AppState::beginMqtt() — попытка подключения к брокеру с сохранёнными настройками.

Цикл (loop)

    AppState::update() — обновляет светодиод (мигание + retained-таймаут), MQTT-клиент и дисплей.

    WebService::handleClient() — обработка HTTP-запросов.

    AppState::getDisplayService().update() — периодическое обновление дисплея.

MQTT-логика

Исходящие сообщения (публикация состояния):

    при локальном изменении светодиода (LedService::set → publishState);

    при команде из MQTT (ON/OFF в cmd-топик → publishState);

    при (пере)подключении, если retained не пришёл за таймаут (1.5 с).

Входящие сообщения (роутинг):

    MqttManager → MqttService::handleMessage → AppState::handleMqttMessage → LedService::handleMqttMessage.

    Если сообщение из state-топика и мы ждём retained — применяется к лампе без публикации.

    Если из cmd-топика и payload ON/OFF — команда применяется и публикуется подтверждение.

Синхронизация с retained-состоянием:

    При (пере)подключении MqttManager вызывает onConnected-колбэк.

    LedService::onMqttConnected подписывается на state-топик и ставит флаг ожидания.

    Если retained пришёл → применяется к лампе.

    Если нет за 1.5 с → публикуется текущее состояние (первый запуск, пустой топик).

    На реконнекте источник истины — брокер. Если локальные изменения не успели уйти в брокер, они будут откатаны к retained.

    Классы — краткое описание

## Ядро

```text
Класс         Назначение
AppState      Композиционный корень. Владеет всеми сервисами, роутит MQTT-сообщения, предоставляет API для WebService и DisplayService.
StateManager  Машина состояний (IDLE, SCANNING, CONNECTING, CONNECTED, AP_MODE, ERROR). Уведомляет через колбэк.
StatusBuilder Собирает JSON-статус (ArduinoJson) и текстовую строку для /status и Serial-логов.
```

## Wi-Fi

```text
Класс              Назначение
WiFiManager        Низкоуровневые операции: scan, connectToNetwork, getEncryptionType.
WiFiCredentials    Хранение SSID/паролей в Preferences (JSON). Миграция из старого текстового формата.
WiFiService Фасад: сканирование, подключение, сохранение/удаление сетей, автоподключение.
```

## MQTT

```text
Класс              Назначение
MqttManager        Обёртка PubSubClient: connect/reconnect с backoff, publish, subscribe, static callbacks.
MqttCredentials    Хранение MQTT-настроек (server/port/user/password/cmdTopic/stateTopic) в JSON.
MqttService Фасад: держит MqttManager + MqttCredentials, предоставляет колбэки onConnected и onMessage.
```

## Периферия

```text
Класс          Назначение
LedManager     Низкоуровневое управление светодиодом: режимы OFF/ON/BLINK_*, учёт activeLow.
LedService     Управление лампой: set/isOn, публикация состояния, retained-синхронизация.
DisplayService OLED SSD1306. Экраны для AP/клиента/сканирования/ошибки, обновление по таймеру и на смену состояния.
```

## Веб

```text
Класс Назначение
WebService HTTP-сервер на порту 80. Отдаёт страницы (AP/клиент) и REST-роуты. HTML встроен в PROGMEM.
```

## Веб-интерфейс: маршруты

```text
Метод  Путь     Назначение
GET    /        HTML-страница (AP или клиент — в зависимости от режима)
GET    /scan    Сканирование Wi-Fi, возвращает JSON массив сетей
POST   /connect Подключение к сети ({ssid, password}), сохранение и перезагрузка
GET    /saved   Список сохранённых SSID
POST   /delete  Удаление сохранённой сети ({index})
GET    /status  JSON-статус (Wi-Fi, IP, RSSI, MQTT, LED, state)
POST   /led     Управление светодиодом ({state: "ON"/"OFF"})
GET    /mqtt    Текущие MQTT-настройки
POST   /mqtt    Сохранение MQTT-настроек, переподключение
POST   /reset   Удаление всех Wi-Fi и MQTT настроек, перезагрузка
POST   /reboot   Перезагрузка устройства
```
