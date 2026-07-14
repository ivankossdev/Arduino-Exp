#include "mShape.h"

void MShape::init() {
  Serial.begin(115200);

  if (!begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  this->xOldPos = 0;
  this->yOldPos = 0;

  this->xPos = 0;
  this->yPos = 16;

  clearDisplay();
  updateDisplay();
}

void MShape::dispCord() {
  setTextSize(1);
  setTextColor(SSD1306_WHITE);
  setCursor(0, 0);
  
  char buf[32];
  snprintf(buf, sizeof(buf), "Pos: X%d Y%d", xPos, yPos);
  print(buf);
}


void MShape::movement(int16_t x, int16_t y) {
  xPos = x; yPos = y; 

  clearSh(off);
  fillRect(x, y, 10, 10, WHITE);
}

void MShape::clearSh(Display dsp) {
  fillRect(xOldPos, yOldPos, 10, 10, BLACK);
  xOldPos = xPos;
  yOldPos = yPos;
}

void MShape::setDefaultXPos() {
  xPos = 0;
}

void MShape::setDefaultYPos() {
  yPos = 16;
}

void MShape::clear(){
  fillRect(xPos, yPos, 10, 10, BLACK);
}

void MShape::drawFrame(bool stopped) {
  const int eraseSize = 12;

  // 1. Стираем старый квадрат
  fillRect(xOldPos, yOldPos, eraseSize, eraseSize, BLACK);
  xOldPos = xPos;
  yOldPos = yPos;

  // 2. Логика пульсации
  int currentSize = 10;
  static bool pulseGrowing = true;

  if (stopped) {
    currentSize = pulseGrowing ? 12 : 8;
    pulseGrowing = !pulseGrowing;
  }

  // 3. Рисуем новый квадрат
  fillRect(xPos, yPos, currentSize, currentSize, WHITE);

  // 4. Стираем строку с координатами ПЕРЕД тем, как рисовать новые
  // Это делает отрисовку предсказуемой: сначала чистый холст, потом текст
  fillRect(0, 0, 128, 16, BLACK);   // 128 — полная ширина экрана

  // 5. Показываем координаты
  dispCord();

  // 6. Отправляем буфер на экран
  display();
}




