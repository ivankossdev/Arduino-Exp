#include <WEMOS_Matrix_LED.h>
/* https://github.com/wemos/WEMOS_Matrix_LED_Shield_Arduino_Library.git */
MLED mled(5); //set intensity=5

void setup() {

}

void loop() {

  mled.dot(0,0); // draw dot
  mled.display();
  delay(1000);
  mled.clear();
  mled.display();
  delay(1000);
}
