#include <Preferences.h>
Preferences preferences;

void setup() {
  Serial.begin(115200);
  
  while (!Serial) {
    delay(100);
  }
  // Инициализация
  preferences.begin("myApp", false);

  // Запись данных
  preferences.putInt("counter", 100);
  preferences.putFloat("voltage", 3.3);
  preferences.putString("wifi_ssid", "MyHomeNetwork");
  preferences.putBool("led_enabled", true);

  Serial.println("Данные записаны");

  // Чтение данных
  int counter = preferences.getInt("counter", 0);
  float voltage = preferences.getFloat("voltage", 0.0);
  String ssid = preferences.getString("wifi_ssid", "Not Set");
  bool ledEnabled = preferences.getBool("led_enabled", false);

  Serial.printf("Counter: %d\n", counter);
  Serial.printf("Voltage: %.2f V\n", voltage);
  Serial.printf("WiFi SSID: %s\n", ssid.c_str());
  Serial.printf("LED Enabled: %s\n", ledEnabled ? "Yes" : "No");

  // Проверка существования ключа
  if (preferences.isKey("counter")) {
    Serial.println("Ключ 'counter' существует");
  }

  // Удаление ключа
  preferences.remove("counter");
  Serial.println("Ключ 'counter' удалён");

  // Завершение работы
  preferences.end();
}

void loop() {}
