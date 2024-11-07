/**
 * WS2812 Lib for mannaNAUTA - ESP32
 * Author	mannaNAUTA
 * Company	mannaTEAM
 * Date		2024-02-19
 */

#include "mannaNAUTA_WS2812.h"

mannaNAUTA_WS2812::mannaNAUTA_WS2812(u16 n /*= 25*/, u8 pin_gpio /*= 47*/, u8 chn /*= 1*/, LED_TYPE t /*= TYPE_GRB*/)
{
	ledCounts = n;
	pin = pin_gpio;
	rmt_chn = chn;
	rmt_mem = RMT_MEM_64;
	br = 255;
	setLedType(t);
}

bool mannaNAUTA_WS2812::begin()
{	
	switch(rmt_chn){
		case 0:
			rmt_mem=RMT_MEM_64;break;
		case 1:
			rmt_mem=RMT_MEM_128;break;
		case 2:
			rmt_mem=RMT_MEM_192;break;
		case 3:
			rmt_mem=RMT_MEM_256;break;
		case 4:
			rmt_mem=RMT_MEM_320;break;
		case 5:
			rmt_mem=RMT_MEM_384;break;
		case 6:
			rmt_mem=RMT_MEM_448;break;
		case 7:
			rmt_mem=RMT_MEM_512;break;		
		default:
			rmt_mem=RMT_MEM_64;break;
	}
	if ((rmt_send = rmtInit(pin, true, rmt_mem)) == NULL){
		return false;
	}
	for(int i=0;i<ledCounts;i++)
	{
		for (int bit = 0; bit < 24; bit++) {
			led_data[i*24+bit].level0 = 1;
			led_data[i*24+bit].duration0 = 4;
			led_data[i*24+bit].level1 = 0;
			led_data[i*24+bit].duration1 = 8;
		}
	}
	realTick = rmtSetTick(rmt_send, 100);
	return true;
}

void mannaNAUTA_WS2812::setLedCount(u16 n)
{
	ledCounts = n;
	begin();
}

void mannaNAUTA_WS2812::setLedType(LED_TYPE t)
{
	rOffset = (t >> 4) & 0x03;
	gOffset = (t >> 2) & 0x03;
	bOffset = t & 0x03;
}

void mannaNAUTA_WS2812::setBrightness(u8 brightness)
{
	br = constrain(brightness, 0, 255);
}

esp_err_t mannaNAUTA_WS2812::setLedColorData(int index, u32 rgb)
{
	return setLedColorData(index, rgb >> 16, rgb >> 8, rgb);
}

esp_err_t mannaNAUTA_WS2812::setLedColorData(int index, u8 r, u8 g, u8 b)
{
	u8 p[3];
	p[rOffset] = r * br / 255;
	p[gOffset] = g * br / 255;
	p[bOffset] = b * br / 255;
	return set_pixel(index, p[0], p[1], p[2]);
}

esp_err_t mannaNAUTA_WS2812::set_pixel(int index, u8 r, u8 g, u8 b)
{
	u32 color = r << 16 | g << 8 | b ;
	for (int bit = 0; bit < 24; bit++) {
		if (color & (1 << (23-bit))) {
			led_data[index*24+bit].level0 = 1;
			led_data[index*24+bit].duration0 = 8;
			led_data[index*24+bit].level1 = 0;
			led_data[index*24+bit].duration1 = 4;
		} else {
			led_data[index*24+bit].level0 = 1;
			led_data[index*24+bit].duration0 = 4;
			led_data[index*24+bit].level1 = 0;
			led_data[index*24+bit].duration1 = 8;
		}
	}
	return ESP_OK;
}

esp_err_t mannaNAUTA_WS2812::setLedColor(int index, u32 rgb)
{
	return setLedColor(index, rgb >> 16, rgb >> 8, rgb);
}

esp_err_t mannaNAUTA_WS2812::setLedColor(int index, u8 r, u8 g, u8 b)
{
	setLedColorData(index, r, g, b);
	return show();
}

esp_err_t mannaNAUTA_WS2812::setAllLedsColorData(u32 rgb)
{
	for (int i = 0; i < ledCounts; i++)
	{
		setLedColorData(i, rgb);
	}
	return ESP_OK;
}

esp_err_t mannaNAUTA_WS2812::setAllLedsColorData(u8 r, u8 g, u8 b)
{
	for (int i = 0; i < ledCounts; i++)
	{
		setLedColorData(i, r, g, b);
	}
	return ESP_OK;
}

esp_err_t mannaNAUTA_WS2812::setAllLedsColor(u32 rgb)
{
	setAllLedsColorData(rgb);
	show();
	return ESP_OK;
}

esp_err_t mannaNAUTA_WS2812::setAllLedsColor(u8 r, u8 g, u8 b)
{
	setAllLedsColorData(r, g, b);
	show();
	return ESP_OK;
}

esp_err_t mannaNAUTA_WS2812::show()
{
	return rmtWrite(rmt_send, led_data, ledCounts*24);
}

uint32_t mannaNAUTA_WS2812::Wheel(byte pos)
{
	u32 WheelPos = pos % 0xff;
	if (WheelPos < 85) {
		return ((255 - WheelPos * 3) << 16) | ((WheelPos * 3) << 8);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return (((255 - WheelPos * 3) << 8) | (WheelPos * 3));
	}
	WheelPos -= 170;
	return ((WheelPos * 3) << 16 | (255 - WheelPos * 3));
}

uint32_t mannaNAUTA_WS2812::hsv2rgb(uint32_t h, uint32_t s, uint32_t v)
{
	u8 r, g, b;
	h %= 360; // h -> [0,360]
	uint32_t rgb_max = v * 2.55f;
	uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

	uint32_t i = h / 60;
	uint32_t diff = h % 60;

	// RGB adjustment amount by hue
	uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

	switch (i) {
	case 0:
		r = rgb_max;
		g = rgb_min + rgb_adj;
		b = rgb_min;
		break;
	case 1:
		r = rgb_max - rgb_adj;
		g = rgb_max;
		b = rgb_min;
		break;
	case 2:
		r = rgb_min;
		g = rgb_max;
		b = rgb_min + rgb_adj;
		break;
	case 3:
		r = rgb_min;
		g = rgb_max - rgb_adj;
		b = rgb_max;
		break;
	case 4:
		r = rgb_min + rgb_adj;
		g = rgb_min;
		b = rgb_max;
		break;
	default:
		r = rgb_max;
		g = rgb_min;
		b = rgb_max - rgb_adj;
		break;
	}
	return (uint32_t)(r << 16 | g << 8 | b);
}

// Limpa faixa (strip)
void mannaNAUTA_WS2812::clear()
{
  setAllLedsColorData(0);
  show();
  vTaskDelay(10 / portTICK_PERIOD_MS);

}

/// Write Word
void mannaNAUTA_WS2812::writeWord(u8 word, u32 rgb)
{
  return writeWord(word, rgb >> 16, rgb >> 8, rgb);
}

void mannaNAUTA_WS2812::writeWord(u8 word, u8 r, u8 g, u8 b)
{

  clear();

  if(word > 0x5A){
    word = word - 0x20;
  }

  switch (word) {
	case 'A':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

	case 'B':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 'C':
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 'D':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 'E':
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;


    case 'F':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(20, r, g, b);
    show();
		break;

    case 'G':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'H':
    setLedColorData(0, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'I':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(7, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'J':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(7, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    show();
		break;
    
    case 'K':
    setLedColorData(0, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(7, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 'L':
    setLedColorData(1, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'M':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'N':
    setLedColorData(0, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'O':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'P':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(20, r, g, b);
    show();
		break;

    case 'Q':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'R':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'S':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'T':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(7, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(22, r, g, b);
    show();
		break;

    case 'U':
    setLedColorData(0, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'V':
    setLedColorData(0, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(22, r, g, b);
    show();
		break;

    case 'W':
    setLedColorData(0, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'X':
    setLedColorData(0, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 'Y':
    setLedColorData(0, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(22, r, g, b);
    show();
		break;

    case 'Z':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

	default:
    setAllLedsColorData(0);
    show();
		break;
	}
	
}

/// Write Number
void mannaNAUTA_WS2812::writeNumber(u8 number, u32 rgb)
{
  return writeNumber(number, rgb >> 16, rgb >> 8, rgb);
}

void mannaNAUTA_WS2812::writeNumber(u8 number, u8 r, u8 g, u8 b)
{

  clear();

  switch (number) {
	case 0:
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

	case 1:
    setLedColorData(2, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(7, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(22, r, g, b);
    show();
		break;

  case 2:
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case 3:
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 4:
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 5:
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 6:
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 7:
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(21, r, g, b);
    show();
		break;

    case 8:
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case 9:
    setLedColorData(1, r, g, b);
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

	default:
    setAllLedsColorData(0);
    show();
		break;
	}
	
}

/// Write Symbol
void mannaNAUTA_WS2812::writeSymbol(u8 symbol, u32 rgb)
{
  return writeSymbol(symbol, rgb >> 16, rgb >> 8, rgb);
}

void mannaNAUTA_WS2812::writeSymbol(u8 symbol, u8 r, u8 g, u8 b)
{
  clear();

  switch (symbol) {
	case '?':
    setLedColorData(2, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(22, r, g, b);
    show();
		break;

	case '#':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(7, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

  case '@':
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case '!':
    setLedColorData(2, r, g, b);
    setLedColorData(7, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(22, r, g, b);
    show();
		break;

    case '$':
    setLedColorData(2, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(7, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(22, r, g, b);
    show();
		break;

    case '%':
    setLedColorData(4, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(20, r, g, b);
    show();
		break;

    case '(':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    show();
		break;

    case ')':
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case '[':
    setLedColorData(0, r, g, b);
    setLedColorData(1, r, g, b);
    setLedColorData(5, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    show();
		break;

    case ']':
    setLedColorData(3, r, g, b);
    setLedColorData(4, r, g, b);
    setLedColorData(9, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case '=]':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case '=[':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

	default:
    setAllLedsColorData(0);
    show();
		break;
	}
}

void mannaNAUTA_WS2812::writeEmotion(u32 emotion, u32 rgb)
{
  return writeEmotion(emotion, rgb >> 16, rgb >> 8, rgb);
}
void mannaNAUTA_WS2812::writeEmotion(u32 emotion, u8 r, u8 g, u8 b)
{

  clear();

  switch (emotion) {
    case '=]':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case '=[':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(6, r, g, b);
    setLedColorData(8, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(20, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

    case '=O':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case '=o':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(11, r, g, b);
    setLedColorData(12, r, g, b);
    setLedColorData(13, r, g, b);
    setLedColorData(15, r, g, b);
    setLedColorData(19, r, g, b);
    setLedColorData(21, r, g, b);
    setLedColorData(22, r, g, b);
    setLedColorData(23, r, g, b);
    show();
		break;

    case '=s':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(14, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(20, r, g, b);
    show();
		break;

    case '=S':
    setLedColorData(1, r, g, b);
    setLedColorData(3, r, g, b);
    setLedColorData(10, r, g, b);
    setLedColorData(16, r, g, b);
    setLedColorData(17, r, g, b);
    setLedColorData(18, r, g, b);
    setLedColorData(24, r, g, b);
    show();
		break;

	default:
    setAllLedsColorData(0);
    show();
		break;
	}

}