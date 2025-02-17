#include <EEPROM.h>
#include "display.h"
#include "img.h"

Display matr(1);  //set intensity=1

void scrollRightDispaly();
void digtalCounter();

char example[512] = {'\0'};

void writeString();
void readString();
void clearString();

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  readString();
}

void loop() {
  writeString();
  matr.scrollLeftString(example, 100);
}

void scrollRightDispaly() {
  matr.insertShape((uint8_t *)arrowRight);
  matr.showDisplay();
  delay(80);
  for (int i = 0; i < 8; i++) {
    matr.scrollRightLoop();
    delay(80);
  }
  matr.clearDisplay();
  delay(1000);
}

void digtalCounter() {
  for (int i = 0; i < 10; i++) {
    matr.inserDigiatl(i | 0x30);
    matr.showDisplay();
    delay(500);
  }
}

void writeString() {
  if (Serial.available() > 0) {
    
    clearString();
    String str = Serial.readString();

    if (str.length() <= 512) {

      unsigned int len = str.length();
      Serial.printf("str length = %d\n", len);
      
      int i = 0;
      do {
        EEPROM.write(i, str[i]);
        i++;
        if(str[i] == '\r' || str[i] == '\n') break;
      } while (str[i] != '\0');
      str[i] = '\0';
      
      if (EEPROM.commit()) {
        Serial.println("EEPROM successfully committed");
        readString();
      } else {
        Serial.println("ERROR! EEPROM commit failed");
      }
    }
  }
}

void readString() {
  int i = 0;
  
  do {
    if (i > 512) {
      Serial.println("Error readString()");
      i = 0;
      break;
    }
    example[i] = (char)EEPROM.read(i);
    i++;
  } while (EEPROM.read(i) != '\0');
  example[i] = '\0';
  Serial.printf("%s\n", example);
}

void clearString() {
  for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }
}
