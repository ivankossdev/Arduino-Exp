

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("\nStart programm");
}

void loop() {
  
  if (Serial.available() > 0) {
    String str = Serial.readString();
    str.trim();
    Serial.println(str);
  }
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(1000);                      
  digitalWrite(LED_BUILTIN, LOW); 
  delay(10);       
}
