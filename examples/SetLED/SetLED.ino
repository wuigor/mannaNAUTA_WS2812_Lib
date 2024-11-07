#include "mannaNAUTA_WS2812.h"

/*
* Total number of LEDs (LED_COUNT), Output pin (LEDS_PIN), and channel (CHANNEL)
* are predefined for the mannaNAUTA board, there is no need to change them.
*
* #define LEDS_COUNT  25
* #define LEDS_PIN    47
* #define CHANNEL     1
*/

mannaNAUTA_WS2812 strip = mannaNAUTA_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

void setup() {

  strip.begin();
  strip.setBrightness(20);  

}

void loop() {
  //Turning on the first and last LED of the strip.

    strip.clear();
    strip.setLedColorData(0, 0, 255, 0);
    strip.show();
    delay(1000);

    strip.clear();
    strip.setLedColorData(24, 0, 255, 0);
    strip.show();
    delay(1000);

}