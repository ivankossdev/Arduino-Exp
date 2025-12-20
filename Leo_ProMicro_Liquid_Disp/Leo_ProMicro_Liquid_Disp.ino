
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

/* 
  Leonardo pro micro
  2 SDA
  3 SCL
 */ 


LiquidCrystal_I2C lcd(0x27,20,2);  
void setup()
{
  lcd.init();                     
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, led");
}


void loop()
{
  lcd.setCursor(0, 1);
  lcd.print(millis()/1000);
}
