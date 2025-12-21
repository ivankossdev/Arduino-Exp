#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "I2C_Device.h"
#include <BH1750.h>

/* 
  Leonardo pro micro
  2 SDA
  3 SCL
 */

int lcdColumns = 16;
int lcdRows = 2;
String devices = "";

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
I2C_Device search;
String messageToScroll = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
BH1750 lightMeter(0x23);

void setup() {
  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
  lcd.init();
  lcd.backlight();
  devices = search.searchDevice();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Display test");
  scrollText(1, devices, 350, lcdColumns);
  lcd.clear();
}


void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Luxmetr: ");
  if (lightMeter.measurementReady()) {
    float lux = lightMeter.readLightLevel();
    lcd.setCursor(0, 1);
    lcd.print(lux);
  }

  // lcd.print("Scroll string");
  // scrollText(1, messageToScroll, 350, lcdColumns);
}

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i = 0; i < lcdColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}
