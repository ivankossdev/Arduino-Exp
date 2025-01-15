#include "string.h"

unsigned char i = 0;
char str[32] = {'\0'};

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
  pinMode(D0, OUTPUT);

}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);   //ON
  digitalWrite(D0, HIGH); 
  delay(100);                      
  digitalWrite(LED_BUILTIN, HIGH);  // OFF
  delay(1000);    
  digitalWrite(D0, LOW);
  delay(1000); 
  sprintf(str, "i = 0x%x", i);
  Serial.println(str);        
  i++;
}
