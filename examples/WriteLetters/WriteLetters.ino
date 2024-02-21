#include "mannaNAUTA_WS2812.h"

/*
* Total number of LEDs (LED_COUNT), Output pin (LEDS_PIN), and channel (CHANNEL)
* are predefined for the mannaNAUTA board, there is no need to change them.
*
* #define LEDS_COUNT  25
* #define LEDS_PIN	  47
* #define CHANNEL		  1
*/

#define COLOR 0xff0000

mannaNAUTA_WS2812 strip = mannaNAUTA_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

void setup() {

  strip.begin();
  strip.setBrightness(20);  

}

void loop() {

  strip.writeWord('M', COLOR);
  delay(1000);
  strip.writeWord('A', COLOR);
  delay(1000);
  strip.writeWord('N', COLOR);
  delay(1000);
  strip.writeWord('N', COLOR);
  delay(1000);
  strip.writeWord('A', COLOR);
  delay(1000);

  for(int i = 65; i <= 90; i++){
      strip.writeWord(i, COLOR);
      delay(1000);
  }

}