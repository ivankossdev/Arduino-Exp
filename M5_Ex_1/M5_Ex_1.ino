#include <M5Unified.h> // Подключаем основную библиотеку

void setup() {
    // Инициализация устройства. auto cfg = M5.config(); позволяет настроить поведение,
    // но для начала можно оставить пустым.
    auto cfg = M5.config();
    M5.begin(cfg);

    // Поворачиваем экран, если это необходимо (опционально)
    // M5.Lcd.setRotation(1); // Раскомментируйте, если текст отображается вверх ногами

    // Очищаем экран и выводим текст
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("Hello, M5 Stick!");
}

void loop() {
    // ОЧЕНЬ ВАЖНО: Обновляем состояние кнопок и других компонентов
    M5.update();

    // Проверяем, нажата ли кнопка A
    if (M5.BtnA.wasPressed()) {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.println("Button A pressed!");
    }

    // Проверяем, нажата ли кнопка B (если она есть на вашей модели)
    if (M5.BtnB.wasPressed()) {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.println("Button B pressed!");
    }

    // Небольшая задержка для стабильности
    delay(50);
}