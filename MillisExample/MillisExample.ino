
unsigned long previousMillis = 0;
const long interval = 500;
int ledState = LOW;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}


void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    Serial.print("previousMillis: ");
    Serial.println(previousMillis);
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // if the LED is off turn it on and vice-versa:
    digitalWrite(LED_BUILTIN, ledState);
  }
}
