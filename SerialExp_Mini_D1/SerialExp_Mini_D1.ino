/* Prototip */ 
void writeString();

/* Init */
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}

/* Main*/
void loop() {
  writeString();
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(1000);                      
  digitalWrite(LED_BUILTIN, LOW); 
  delay(1);       
}

/* Func realistation */
void writeString(){
  if (Serial.available() > 0) {
    String str = Serial.readString();
    str.trim();
    int i = 0; 
    do{
      Serial.printf("0x%x - %c\n\r", str[i], str[i]);
      i++;
    }while(str[i] !='\0');
  }
}