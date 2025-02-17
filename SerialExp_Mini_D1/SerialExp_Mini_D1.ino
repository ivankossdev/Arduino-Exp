#include <EEPROM.h>


/* Prototip */
void writeString();
void readString();
void clearString();

/* Init */
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  EEPROM.begin(512);
}

/* Main*/
void loop() {
  writeString();
  readString();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1);
}

/* Func realistation */
void writeString() {
  if (Serial.available() > 0) {
    clearString();
    String str = Serial.readString();
    if (str.length() <= 512) {
      Serial.printf("str length = %d\n", str.length());
      str.trim();
      int i = 0;
      do {
        EEPROM.write(i, str[i]);
        i++;
      } while (str[i] != '\0');
      if (EEPROM.commit()) {
        Serial.println("EEPROM successfully committed");
      } else {
        Serial.println("ERROR! EEPROM commit failed");
      }
    }
  }
}

void readString() {
  int i = 0;
  char str[512] = { '\0' };
  do {
    if (i > 512) {
      Serial.println("Error readString()");
      i = 0;
      break;
    }
    str[i] = (char)EEPROM.read(i);
    i++;
  } while (EEPROM.read(i) != 0);
  str[i] = '\0';
  Serial.println(str);
}

void clearString() {
  for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }
}

//Lorem ipsum dolor sit amet, consectetur adipiscing elit.
