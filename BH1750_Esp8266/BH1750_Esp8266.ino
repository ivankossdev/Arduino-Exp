#include <BH1750.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BH1750 lightMeter(0x23);

void setup() {

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  
}

void loop() {
  if (lightMeter.measurementReady()) {
    float lux = lightMeter.readLightLevel();
    
    if(lux < 60){
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
    }
    else{
      digitalWrite(LED_BUILTIN, HIGH);
    }
    display.clearDisplay();
    display.setCursor(0, 25);  
    display.printf("lux %.2f", lux);
    display.display();
    //   Serial.print("Light: ");
    //   Serial.print(lux);
    //   Serial.println(" lx");
  }
}
