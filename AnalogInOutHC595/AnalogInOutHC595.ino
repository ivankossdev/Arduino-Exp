#include "MyHW595.h"

/*
Arduino  Leonardo
         board ->    <- HC595
         MOSI 16    14 | DS    | Data pin
         SCK  15    11 | SH_CP | Clock
         SS   10    12 | ST_CP | Storage Registet input

ESP8266
         board ->    <- HC595
         MOSI D7    14 | DS    | Data pin
         SCK  D5    11 | SH_CP | Clock
         SS   D4    12 | ST_CP | Storage Registet input

Resistor connect
      PIN  1 | GND  
      PIN  2 | A0
      PIN  3 | VCC +3V
*/

#define SS D4

MYHW595 hw595(SS);

void setup() {
  Serial.begin(115200);
  hw595.WriteByte(0x00);
}

void loop() {
  int sensorValue = analogRead(A0);
  int res = sensorValue / 4;
  Serial.print("Analog value: ");
  Serial.println(res);
  if(res >= 256) res = 255;
  hw595.WriteByte((char)res);
  delay(250);
}