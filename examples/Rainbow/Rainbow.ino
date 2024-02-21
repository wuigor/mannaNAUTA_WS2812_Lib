#include "mannaNAUTA_WS2812.h"

/*
* Total number of LEDs (LED_COUNT), Output pin (LEDS_PIN), and channel (CHANNEL)
* are predefined for the mannaNAUTA board, there is no need to change them.
*
* #define LEDS_COUNT  25
* #define LEDS_PIN	  47
* #define CHANNEL		  1
*/

mannaNAUTA_WS2812 strip = mannaNAUTA_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

void setup() {
  strip.begin();
  strip.setBrightness(20);  
}

void loop() {

  for (int j = 0; j < 255; j += 2) {

    for (int i = 0; i < LEDS_COUNT; i++) {
      strip.setLedColorData(i, strip.Wheel((i * 256 / LEDS_COUNT + j) & 255));
    }
    
    strip.show();
    delay(10);

  }

}