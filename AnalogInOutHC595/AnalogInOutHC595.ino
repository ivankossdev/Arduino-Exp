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

// Сдвигает 1 бит влево
void ShiftLeft() {
  for (uint8_t i = 0; i < 8; i++) {
    int sensorValue = analogRead(A0);
    hw595.WriteByte(1 << i);
    delay(sensorValue / 4);
  }
}

// Сдвигает 1 бит вправо
void ShiftRight(){
  for(int i = 7; i >= 0; i--){
    int sensorValue = analogRead(A0);
    hw595.WriteByte(1 << i);
    delay(sensorValue / 4);
  }
}

/*
  Example_1() функция считывает данные АЦП с A0 от 0-1024
  и передает значение на сдвиговый регистр 74hc595 результат 
*/
void Example_1(){
  int sensorValue = analogRead(A0); // Читает значение на A0 
  int res = sensorValue / 4;        // значение sensorValue / 4 нужно что бы правильно отображать данные на
                                    // сдвиговом регистре 74hc595, его диапазон 1 байт 0-255
  Serial.print("Analog value: ");   // Отправляет полученный результат на шину Serial RS232
  Serial.println(res);
  if(res >= 256) 
    res = 255;
  hw595.WriteByte((char)res);
  delay(250);
}

void Example_2(){
  ShiftLeft();
  ShiftRight();
}

void setup() {
  // Serial.begin(115200);
  hw595.WriteByte(0x00);
}

void loop() {
  
  Example_2();
}