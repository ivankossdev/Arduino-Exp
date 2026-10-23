#include <Adafruit_NeoPixel.h>
#include "matr.h"
#include "config.h"

/* == матрица 4х4 ==

  0  1  2  3 | ряд 1
  4  5  6  7 | ряд 2
 11 10  9  8 | ряд 3
 12 13 14 15 | ряд 4
 
 =================== */

static Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int ledArray[12] = { 0, 1, 2, 3, 4, 11, 12, 13, 14, 15, 8, 7 };

void matrInit() {
  ring.begin();
  ring.setBrightness(3);
}

// Проверка в каком ряду находится включенный диод
int rowChecker(int pos) {
  if (pos >= 0 && pos <= 3) return 1;
  else if (pos >= 4 && pos <= 7) return 2;
  else if (pos >= 8 && pos <= 11) return 3;
  else if (pos >= 12 && pos <= 15) return 4;
  else return 0;
}

void dotWalk(){
    for (int i = 0; i < COUNTER_LINE; i++) {
    ring.clear();
    ring.setPixelColor(ledArray[i], ring.Color(0, 0, 255));
    ring.show();
    Serial.print("Led position ");
    Serial.println(rowChecker(ledArray[i]));
    delay(2000);
  }
}

