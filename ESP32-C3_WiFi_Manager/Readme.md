```text
ESP32-C3_WiFi_Manager/
├── src/
│   ├── main.cpp
│   ├── Menu/
│   │   ├── Menu.h
│   │   └── Menu.cpp
│   ├── WiFi/
│   │   ├── WiFiManager.h
│   │   ├── WiFiManager.cpp
│   │   ├── WiFiCredentials.h
│   │   └── WiFiCredentials.cpp
│   ├── Web/
│   │   ├── WebServer.h
│   │   └── WebServer.cpp
│   ├── MQTT/
│   │   ├── MqttClient.h
│   │   └── MqttClient.cpp
│   ├── OTA/
│   │   ├── OtaManager.h
│   │   └── OtaManager.cpp
│   ├── StateMachine/
│   │   ├── AppState.h
│   │   └── AppState.cpp
│   └── Utils/
│       ├── Logger.h
│       ├── Config.h
│       └── Helpers.h
├── data/                 (для SPIFFS/LittleFS – веб-страницы)
│   ├── index.html
│   ├── style.css
│   └── script.js
├── platformio.ini        (или .ino, но лучше PlatformIO)
└── README.md
```
