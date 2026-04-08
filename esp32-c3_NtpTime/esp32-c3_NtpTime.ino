#include <Wire.h>
#include "wifi_manager.h"
#include "ntp_time.h"
#include "display_manager.h"
#include "time_utils.h"
#include "config.h"


// Создаём объекты
WiFiManager wifiManager;
NTPTime ntpTime;
DisplayManager displayManager;

void setup() {
    Serial.begin(115200);

    // Инициализация дисплея
    displayManager.init();

    // Показываем подключение к Wi‑Fi
    displayManager.showConnecting();

    // Подключаемся к Wi‑Fi
    if (wifiManager.connect()) {
        displayManager.showConnected();

        // Запускаем NTP‑клиент
        ntpTime.begin();
    } else {
        // Если не подключились, показываем ошибку
        displayManager.clear();
        displayManager.setTextSize(1);
        displayManager.setCursor(0, 0);
        displayManager.println("WiFi Error!");
        displayManager.refreshDisplay();
        while (true); // Зависаем при ошибке
    }
}


void loop() {
    // Обновляем время от NTP‑сервера
    ntpTime.update();

    // Получаем Unix‑время
    unsigned long epochTime = ntpTime.getEpochTime();

    // Преобразуем в дату
    int currentYear, currentMonth, currentDay;
    TimeUtils::unixToDate(epochTime, currentYear, currentMonth, currentDay);

    // Получаем время
    int currentHour = ntpTime.getHour();
    int currentMinute = ntpTime.getMinute();
    int currentSecond = ntpTime.getSecond();

    // Обновляем дисплей
    displayManager.updateTime(
        currentHour,
        currentMinute,
        currentSecond,
        currentDay,
        currentMonth,
        currentYear
    );

    delay(1000); // Обновляем каждую секунду
}
