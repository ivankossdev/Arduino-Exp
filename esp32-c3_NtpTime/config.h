#ifndef CONFIG_H
#define CONFIG_H

// Объявления констант (без значений)
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const char* NTP_SERVER;
extern const long TIME_OFFSET;
extern const unsigned long UPDATE_INTERVAL;

// Настройки дисплея
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DISPLAY_ADDRESS 0x3C

#endif

