void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  static bool state = true; // static сохраняет значение между вызовами loop()

  for (int i = 0; i < 10; i++) {
    // 1. Применяем состояние к светодиоду
    digitalWrite(LED_BUILTIN, state ? HIGH : LOW); // или просто state
    
    // 2. Инвертируем флаг для следующего шага (true -> false -> true)
    state = !state;

    // 3. Выводим инфо в порт
    Serial.print("Count ");
    Serial.println(i);

    // 4. Пауза 1 секунда (одна для всех)
    delay(1000);
  }
  
  Serial.println();
}
