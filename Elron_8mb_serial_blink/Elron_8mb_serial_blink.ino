bool state = true;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    if (state) {
      digitalWrite(LED_BUILTIN, HIGH);
      state = false;
      delay(1000);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      state = true;
      delay(1000);
    }
    Serial.print("Count ");
    Serial.println(i);
  }
  Serial.println();
}
