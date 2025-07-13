#include <SPI.h>

/*
Leonardo
Arduino board ->    <- HC595
         MOSI 16    14 | DS    | Data pin
         SCK  15    11 | SH_CP | Clock
         SS   10    12 | ST_CP | Storage Registet input
*/

const int latchPin = 10; // SS pin (ST_CP on HC595)

void setup() {
  pinMode(latchPin, OUTPUT);
  SPI.begin();
}

void WriteByte(uint8_t data){
  digitalWrite(latchPin, LOW);
  SPI.transfer(data); 
  digitalWrite(latchPin, HIGH);
}

void loop() {
  for(uint8_t i = 0; i < 8; i++){
    WriteByte(1 << i);
    delay(100);
  }
}
