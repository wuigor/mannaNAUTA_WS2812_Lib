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

u8 m_color[5][3] = { {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 255}, {0, 0, 0} };
int delayval = 100;

void setup() {

	strip.begin();
	strip.setBrightness(20);

}

void loop() {

	for (int j = 0; j < 5; j++) {

		for (int i = 0; i < LEDS_COUNT; i++) {

			strip.setLedColorData(i, m_color[j][0], m_color[j][1], m_color[j][2]);
			strip.show();
			delay(delayval);

		}

		delay(500);

	}

}