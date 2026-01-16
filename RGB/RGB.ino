#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 8
#define DELAYVAL 1000  // Time (in milliseconds) to pause between pixels

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.clear();
  pixels.show();
}

void loop() {

  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    pixels.setPixelColor(i, pixels.Color(i + 2, i + 1, 0));
    pixels.show();
    delay(10);
  }

  delay(DELAYVAL);
  pixels.clear();
  pixels.show();
  delay(DELAYVAL);
  pixels.clear();
  pixels.show();
}
