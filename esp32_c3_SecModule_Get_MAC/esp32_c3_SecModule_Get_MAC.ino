#include <WiFi.h>
#include <Bounce2.h>
#define SENSOR_PIN 9 // Кнопка на ESP32 
#define LAMP_PIN 8   // Встроенный светодиод на ESP32 

// MAC: 44:B1:76:05:0F:78 этого контроллера  0x44, 0xB1, 0x76, 0x05, 0x0F, 0x78
// MAC: 90:E5:B1:6A:8C:1C модуль управления 
Bounce pressButton = Bounce();

// Состояние лампы 
bool lampState = false; 

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // Инициализация кнопки
  pinMode(SENSOR_PIN, INPUT_PULLUP);

  // Обработчик нажатий кнопки
  pressButton.attach(SENSOR_PIN);
  pressButton.interval(25);

  // Инициализация сетодиода (выключено)
  pinMode(LAMP_PIN, OUTPUT);
  digitalWrite(LAMP_PIN, HIGH);
}

unsigned long previousPrint = 0;
const int NONBLOCKDELAY = 30000;

void loop() {

  // Не блокирующий принт каждую секунду
  unsigned long currentTime = millis();
  if(currentTime - previousPrint >= NONBLOCKDELAY){
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    previousPrint = currentTime; 
  }
  
  // Нажатие кнопки
  pressButton.update();
  if (pressButton.fell()) {
    if(!lampState){
      digitalWrite(LAMP_PIN, LOW);
      lampState = true;
      Serial.println("btn BOOT GPIO_9 LAMP ON");
    }
    else{
      digitalWrite(LAMP_PIN, HIGH);
      lampState = false;
      Serial.println("btn BOOT GPIO_9 LAMP OFF");
    }
  }
}