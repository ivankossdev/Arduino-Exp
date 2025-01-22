#include <BH1750.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BH1750 lightMeter(0x23);
Adafruit_AHTX0 aht;

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
  // Init AHT10 Sensor
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  delay(2000);
  display.clearDisplay();
  
  display.setTextColor(SSD1306_WHITE); // Draw white text
  
}

void loop() {
  if (lightMeter.measurementReady()) {
    float lux = lightMeter.readLightLevel();
    
    if(lux > 40 && lux < 60){
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
    }
    else if(lux < 40){
      digitalWrite(LED_BUILTIN, LOW);
      delay(25);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
    }
    else{
      digitalWrite(LED_BUILTIN, HIGH);
    }
    display.clearDisplay();
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.printf("Temperature: %.2f C", temp.temperature);

    display.setCursor(0, 9);
    display.printf("Humidity: %.2f rH", humidity.relative_humidity);

    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setCursor(0, 18);  
    display.printf("lux %.2f", lux);
    display.display();

    delay(250);
  }
}
