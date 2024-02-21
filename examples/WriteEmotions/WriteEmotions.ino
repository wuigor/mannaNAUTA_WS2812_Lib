#include "mannaNAUTA_WS2812.h"

/*
* Total number of LEDs (LED_COUNT), Output pin (LEDS_PIN), and channel (CHANNEL)
* are predefined for the mannaNAUTA board, there is no need to change them.
*
* #define LEDS_COUNT  25
* #define LEDS_PIN	  47
* #define CHANNEL		  1
*/

#define COLOR 0x00FF00

mannaNAUTA_WS2812 strip = mannaNAUTA_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

void setup() {

  strip.begin();
  strip.setBrightness(20);  

}

void loop() {

  strip.writeEmotion('=]', COLOR);
  delay(3000);

  strip.writeEmotion('=[', COLOR);
  delay(3000);

  strip.writeEmotion('=o', COLOR);
  delay(3000);

  strip.writeEmotion('=s', COLOR);
  delay(1000);

  strip.writeEmotion('=S', COLOR);
  delay(3000);

}