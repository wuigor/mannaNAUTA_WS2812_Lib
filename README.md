# mannaNAUTA WS2812 Addressable LED Library

This library provides an easy-to-use interface for controlling WS2812 addressable LEDs. 

WS2812 LEDs are individually addressable RGB LEDs, allowing for a wide range of colorful and dynamic lighting effects.

## Features
* Control individual WS2812 LEDs or groups of LEDs;
* Set colors, brightness, and animation effects;
* Display the letters A-Z on a strip of 25 LEDs;
* Display the numbers 0-9 on a strip of 25 LEDs;
* Display the following symbols on a strip of 25 LEDs: ?, #, @, !, $, %, (, ), [, ];
* Display the following emotions on a strip of 25 LEDs: =], =[, =O, =S.
* Compatible with mannaNAUTA board.

Based on the example program "led_strip" in IDF-SDK and lib "WS2812 Lib For ESP32" in Freenove. The source code repository is here:

https://github.com/espressif/esp-idf/tree/master/examples/peripherals/rmt/led_strip

https://github.com/Freenove/Freenove_WS2812_Lib_for_ESP32

## Examples:
* Turning on the first and last LED of the strip.

```C
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

    strip.clear();
    strip.setLedColorData(0, 0, 255, 0);
    strip.show();
    delay(1000);

    strip.clear();
    strip.setLedColorData(24, 0, 255, 0);
    strip.show();
    delay(1000);

}
```

* Rainbow
```C
#include "mannaNAUTA_WS2812.h"

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
```

* RGBW

```C
#include "mannaNAUTA_WS2812.h"

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
```

## Usage

Total number of LEDs (LED_COUNT), Output pin (LEDS_PIN), and channel (CHANNEL) are predefined for the mannaNAUTA board, there is no need to change them.

If necessary, you can change them:
```C
#define LEDS_COUNT  25
#define LEDS_PIN	47
#define CHANNEL		1
```

* Construction. </br> Create a strip object.
```C
mannaNAUTA_WS2812 strip = mannaNAUTA_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);
```

* Initialization
```C
strip.begin();
```

* Set brightness
```C
strip.setBrightness(20);  
```

* Set Color LED
    * Send the color data of the specified LED to the controller.
    * Display color change after calling show function.
```C
strip.setLedColorData(id, color);
strip.show();
```

or

```C
strip.setLedColorData(id, r, g, b);
strip.show();
```

* Write a letter

```C
strip.writeWord('A', color);
```

* Write a number

```C
strip.writeNumber(1, color);
```

* Write a symbol

```C
strip.writeSymbol('@', color);
```

* Write a emotion

```C
strip.writeEmotion('=]', COLOR);
```

