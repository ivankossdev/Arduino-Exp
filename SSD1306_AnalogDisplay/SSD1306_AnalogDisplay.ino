#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define CENTER_X 64
#define CENTER_Y 32
#define CLOCK_RADIUS 30

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Переменные времени
int hours = 1;
int minutes = 15;
int seconds = 0;
unsigned long lastTime = 0;

// Структура для стрелок
struct Hand {
  float length;
  float width;
  uint16_t color;
};

Hand hourHand = {15, 3, SSD1306_WHITE};
Hand minuteHand = {20, 2, SSD1306_WHITE};
Hand secondHand = {25, 1, SSD1306_WHITE};

void setup() {
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1309 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  drawClockFace();
  updateTime();
  display.display();
  
  lastTime = millis();
}

void loop() {
  // Обновляем время каждую секунду
  if(millis() - lastTime >= 1000) {
    lastTime = millis();
    seconds++;
    
    if(seconds >= 60) {
      seconds = 0;
      minutes++;
      if(minutes >= 60) {
        minutes = 0;
        hours++;
        if(hours >= 12) hours = 0;
      }
    }
    
    updateTime();
  }
}

// Рисуем циферблат
void drawClockFace() {
  display.clearDisplay();
  
  // Внешний круг
  display.drawCircle(CENTER_X, CENTER_Y, CLOCK_RADIUS, SSD1306_WHITE);
  
  // Часовые метки
  for(int i = 0; i < 12; i++) {
    float angle = i * PI/6 - PI/2; // 30 градусов между метками
    int innerX = CENTER_X + (CLOCK_RADIUS - 4) * cos(angle);
    int innerY = CENTER_Y + (CLOCK_RADIUS - 4) * sin(angle);
    int outerX = CENTER_X + CLOCK_RADIUS * cos(angle);
    int outerY = CENTER_Y + CLOCK_RADIUS * sin(angle);
    
    display.drawLine(innerX, innerY, outerX, outerY, SSD1306_WHITE);
    
    // Цифры (для 3, 6, 9, 12)
    if(i % 3 == 0) {
      int numX = CENTER_X + (CLOCK_RADIUS - 10) * cos(angle) - 3;
      int numY = CENTER_Y + (CLOCK_RADIUS - 10) * sin(angle) - 3;
      
      display.setCursor(numX, numY);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      
      int number = (i == 0) ? 12 : i;
      display.print(number);
    }
  }
  
  // Минутные метки
  for(int i = 0; i < 60; i++) {
    if(i % 5 != 0) { // Пропускаем часовые метки
      float angle = i * PI/30 - PI/2;
      int x1 = CENTER_X + (CLOCK_RADIUS - 2) * cos(angle);
      int y1 = CENTER_Y + (CLOCK_RADIUS - 2) * sin(angle);
      int x2 = CENTER_X + CLOCK_RADIUS * cos(angle);
      int y2 = CENTER_Y + CLOCK_RADIUS * sin(angle);
      
      display.drawPixel(x1, y1, SSD1306_WHITE);
      display.drawPixel(x2, y2, SSD1306_WHITE);
    }
  }
}

// Рисуем стрелку
void drawHand(float angle, Hand hand) {
  int x = CENTER_X + hand.length * cos(angle);
  int y = CENTER_Y + hand.length * sin(angle);
  
  // Основная линия стрелки
  display.drawLine(CENTER_X, CENTER_Y, x, y, hand.color);
  
  // Утолщение стрелки (рисуем дополнительные линии для ширины)
  if(hand.width > 1) {
    for(int i = 1; i <= hand.width/2; i++) {
      // Смещение перпендикулярно направлению стрелки
      float perpAngle = angle + PI/2;
      int dx = i * cos(perpAngle);
      int dy = i * sin(perpAngle);
      
      display.drawLine(CENTER_X + dx, CENTER_Y + dy, x + dx, y + dy, hand.color);
      display.drawLine(CENTER_X - dx, CENTER_Y - dy, x - dx, y - dy, hand.color);
    }
  }
  
  // Центральная точка
  display.fillCircle(CENTER_X, CENTER_Y, 3, SSD1306_WHITE);
}

// Обновляем время и стрелки
void updateTime() {
  // Пересчитываем углы
  float secondAngle = seconds * PI/30 - PI/2;
  float minuteAngle = minutes * PI/30 - PI/2 + seconds * PI/1800; // Плавное движение
  float hourAngle = hours * PI/6 - PI/2 + minutes * PI/360; // Плавное движение
  
  // Рисуем циферблат
  drawClockFace();
  
  // Рисуем стрелки (в порядке: часовая, минутная, секундная)
  drawHand(hourAngle, hourHand);
  drawHand(minuteAngle, minuteHand);
  
  // Секундная стрелка красным (если поддерживается цвет)
  display.drawLine(CENTER_X, CENTER_Y, 
                   CENTER_X + secondHand.length * cos(secondAngle),
                   CENTER_Y + secondHand.length * sin(secondAngle),
                   SSD1306_WHITE);
  
  // Центральная точка
  display.fillCircle(CENTER_X, CENTER_Y, 3, SSD1306_WHITE);
  
  // Отображаем цифровое время внизу
  // display.setCursor(40, 55);
  // display.setTextSize(1);
  // display.print(formatTime(hours));
  // display.print(":");
  // display.print(formatTime(minutes));
  // display.print(":");
  // display.print(formatTime(seconds));
  
  display.display();
}

// Форматирование времени (добавляем ведущий ноль)
// String formatTime(int value) {
//   if(value < 10) {
//     return "0" + String(value);
//   }
//   return String(value);
// }