
unsigned long previousMillis = 0;
const long interval = 500;
int ledState = LOW;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}


void loop() {
  // MasterSerial();
  SlaveSerial();
}

void MasterSerial() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Serial.print("previousMillis: ");
    // Serial.println(previousMillis);
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
      Serial.print(1);
    } else {
      ledState = LOW;
      Serial.print(0);
    }
    digitalWrite(LED_BUILTIN, ledState);
  }
}

void SlaveSerial() {
  

  if (Serial.available() > 0) {

    char c = Serial.read();
    if (c == '1') { digitalWrite(LED_BUILTIN, LOW); }
    if (c == '0') { digitalWrite(LED_BUILTIN, HIGH); }
    
    Serial.print("I received: ");
    Serial.println(c, DEC);
  }
  
}
