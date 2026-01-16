#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUM_LEDS 8

Adafruit_NeoPixel ring(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  ring.begin();
  ring.show();
}

void loop() {
  rainbowBreathing(10);
}

void rainbowBreathing(uint8_t wait) {
  static uint16_t hueBase = 0;
  hueBase += 20;  // rainbow shift

  // Smooth breathing brightness: 0 → 255 → 0
  float breathe = (exp(sin(millis() / 1000.0 * PI)) - 0.36787944) * 108.0;

  for (int i = 0; i < NUM_LEDS; i++) {
    uint16_t hue = hueBase + (i * 65536L / NUM_LEDS);
    uint32_t color = ring.ColorHSV(hue, 255, (uint8_t)breathe);
    ring.setPixelColor(i, ring.gamma32(color));
  }

  ring.show();
  delay(wait);
}