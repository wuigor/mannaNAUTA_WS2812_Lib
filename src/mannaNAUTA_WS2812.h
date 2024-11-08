/**
 * WS2812 Lib for mannaNAUTA - ESP32
 * Author	mannaNAUTA
 * Company	mannaTEAM
 * Date		2024-11-08
 */

#ifndef _MANNANAUTA_WS2812_h
#define _MANNANAUTA_WS2812_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#ifndef LEDS_PIN
#define LEDS_PIN	47
#endif

#ifndef LEDS_COUNT
#define LEDS_COUNT  25
#endif

#ifndef CHANNEL
#define CHANNEL		1
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "Arduino.h"

#include "esp32-hal.h"

//Modify the definition to expand the number of leds
//Supports a maximum of 1100 leds
#define NR_OF_LEDS   256  

#define NR_OF_ALL_BITS 24*NR_OF_LEDS

enum LED_TYPE
{					  //R  G  B
	TYPE_RGB = 0x06,  //00 01 10
	TYPE_RBG = 0x09,  //00 10 01
	TYPE_GRB = 0x12,  //01 00 10
	TYPE_GBR = 0x21,  //10 00 01
	TYPE_BRG = 0x18,  //01 10 00
	TYPE_BGR = 0x24	  //10 01 00
};

class mannaNAUTA_WS2812
{
protected:
	
	uint16_t ledCounts;
	uint8_t pin;
	uint8_t br;
	uint8_t rmt_chn;
	
	uint8_t rOffset;
	uint8_t gOffset;
	uint8_t bOffset;
	
	float realTick;
	rmt_reserve_memsize_t rmt_mem;
	rmt_data_t led_data[NR_OF_ALL_BITS];

public:
	mannaNAUTA_WS2812(uint16_t n = 8, uint8_t pin_gpio = 2, uint8_t chn = 0, LED_TYPE t = TYPE_GRB);

	bool begin();
	void setLedCount(uint16_t n);
	void setLedType(LED_TYPE t);
	void setBrightness(uint8_t brightness);

	void clear();

	void writeWord(uint8_t word, uint32_t rgb);
	void writeWord(uint8_t word, uint8_t r, uint8_t g, uint8_t b);

	void writeNumber(uint8_t number, uint32_t rgb);
	void writeNumber(uint8_t number, uint8_t r, uint8_t g, uint8_t b);

	void writeSymbol(uint8_t symbol, uint32_t rgb);
	void writeSymbol(uint8_t symbol, uint8_t r, uint8_t g, uint8_t b);

	void writeEmotion(uint32_t emotion, uint32_t rgb);
	void writeEmotion(uint32_t emotion, uint8_t r, uint8_t g, uint8_t b);

	esp_err_t set_pixel(int index, uint8_t r, uint8_t g, uint8_t b);
	
	esp_err_t setLedColorData(int index, uint32_t rgb);
	esp_err_t setLedColorData(int index, uint8_t r, uint8_t g, uint8_t b);

	esp_err_t setLedColor(int index, uint32_t rgb);
	esp_err_t setLedColor(int index, uint8_t r, uint8_t g, uint8_t b);

	esp_err_t setAllLedsColorData(uint32_t rgb);
	esp_err_t setAllLedsColorData(uint8_t r, uint8_t g, uint8_t b);

	esp_err_t setAllLedsColor(uint32_t rgb);
	esp_err_t setAllLedsColor(uint8_t r, uint8_t g, uint8_t b);

	esp_err_t show();

	uint32_t Wheel(byte pos);
	uint32_t hsv2rgb(uint32_t h, uint32_t s, uint32_t v);
};

#endif

