#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
#define NUM_MODES 8

//#define DEBUG

		int	step			= 0;
		int	global_brightness	= 64;
const		byte	interrupt_pin		= 2;
volatile	int	mode			= 0;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} ProColor;

ProColor colors[5] =	{	{   0, 143, 255 },	// mega episches Blau
				{ 255, 125,   0 },	// Admin - Orange
				{  40, 255, 150 },	// angenehmes Grün
				{ 176, 255,   5 },	// Olivgrün des Friedens
				{ 255,   0, 130 }	// altes Pink
			};


// Adafruit_NeoPixel(  20, PIN, NEO_GRB + NEO_KHZ800);
//		       ^    ^ 	   ^           ^
//		       |    |      |           +------ bitstream speed  [NEO_KHZ800, NEO_KHZ400]
//		       |    |      +------------------ color order	[NEO_GRB, NEO_RGB]
//		       |    +------------------------- pin number where the data pin is connected
//		       +------------------------------ number of pixels in strip
//
Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
	// Setup an Interrupt to listen for button press
	pinMode(interrupt_pin, INPUT);
	attachInterrupt(digitalPinToInterrupt(interrupt_pin),cycle_mode, RISING);

	// Setup the LED-strip
	strip.begin();
	strip.show(); // Initialize all pixels to 'off'
#ifdef DEBUG
	Serial.begin(9600);
#endif // DEBUG
}

void loop() {
	strip.setBrightness(global_brightness);
	int color_index = -1;
	switch (mode)
	{
	case 0:
		rainbow(step);
		break;
	case 1:
		color_index = 0; // Mega episches Blau
		pulseColor(step, colors[color_index].r, colors[color_index].g, colors[color_index].b);
		break;
	case 2:
		color_index = 1; // Admin Orange
		pulseColor(step, colors[color_index].r, colors[color_index].g, colors[color_index].b);
		break;
	case 3:
		color_index = 2; // Angenehmes Grün
		pulseColor(step, colors[color_index].r, colors[color_index].g, colors[color_index].b);
		break;
	case 4:
		color_index = 3; // Olivgrün des Friedens
		pulseColor(step, colors[color_index].r, colors[color_index].g, colors[color_index].b);
		break;
	case 5:
		color_index = 4; // Altes Pink
		pulseColor(step, colors[color_index].r, colors[color_index].g, colors[color_index].b);
		break;
	case 6:
		pr0ColorWheel(step);
		break;
	case 7:
		larsonStar(step);
	default:
		break;
	}
	delay(10000/255);
	step = ++step % 255;
}

void cycle_mode() {
	static long current_millis = millis() + 501;
	if (millis() - current_millis >= 500) {
		mode = ++mode % NUM_MODES;
		current_millis = millis();
#ifdef DEBUG
		Serial.print("Mode changed to: ");
		Serial.println(mode);
#endif // DEBUG
		if (mode > 5 && mode < 1) {
			step = 0;
		}
	}
}

void pulseColor(uint8_t step, uint8_t r, uint8_t g, uint8_t b) {
	int current_brightness = 0;
	current_brightness = round(global_brightness * sin(PI / 255.0*step));
	strip.setBrightness(current_brightness);

	for (uint16_t i = 0; i < strip.numPixels(); i++) {
		strip.setPixelColor(i, strip.Color(r, g, b));
		strip.show();
	}
}

void pr0ColorWheel(uint8_t step) {
	double angle		= 0;
	double angle_offset	= 0;
	double led_angle	= 0;
	int num_colors		= 5;		// FIXME
	angle_offset = (360.0 / 255.0) * step;
	for (int i = 0; i < strip.numPixels(); i++) {
		angle = 360 / strip.numPixels() * i;
		led_angle = (round(angle) + round(angle_offset)) % 360;
#ifdef DEBUG
		if (i == 1)
		{
			Serial.println(angle_offset);
		}
#endif // DEBUG
		if ( led_angle >= 0 * 360 / num_colors && led_angle < 1 * 360 / num_colors) {
			strip.setPixelColor(i, strip.Color(colors[0].r, colors[0].g, colors[0].b));
		} else if (led_angle >= 1 * 360 / num_colors && led_angle < 2 * 360 / num_colors) {
			strip.setPixelColor(i, strip.Color(colors[1].r, colors[1].g, colors[1].b));
		} else if (led_angle >= 2 * 360 / num_colors && led_angle < 3 * 360 / num_colors) {
			strip.setPixelColor(i, strip.Color(colors[2].r, colors[2].g, colors[2].b));
		} else if (led_angle >= 3 * 360 / num_colors && led_angle < 4 * 360 / num_colors) {
			strip.setPixelColor(i, strip.Color(colors[3].r, colors[3].g, colors[3].b));
		} else if (led_angle >= 4 * 360 / num_colors && led_angle < 5 * 360 / num_colors) {
			strip.setPixelColor(i, strip.Color(colors[4].r, colors[4].g, colors[4].b));
		} else {
			strip.setPixelColor(i, strip.Color(128, 128, 128));
		}
	}
	strip.show();
}

void larsonStar(uint8_t step) {
	double	angle			= 0;
	double	angle_offset		= 0;
	double	led_angle		= 0;
	float	min_brightness		= 0.1;
	float	current_brightness	= 0.0;
	int	num_colors		= 5;		// FIXME
	int	color_index		= 0;
	int	wheel_section		= 360 / num_colors;
	
	angle_offset = (360.0 / 255.0) * step;
	int r, g, b;
	for (int i = 0; i < strip.numPixels(); i++) {
		angle			= 360 / strip.numPixels() * i;
		led_angle		= (round(angle) + round(angle_offset)) % 360;
		current_brightness	= 1.0 - ((fmod(led_angle, wheel_section))* (((float)(1.0 - min_brightness) / float(wheel_section))));
#ifdef DEBUG
		if (i == 1)
		{
			Serial.print("Led angle: ");
			Serial.print(led_angle);
			Serial.print(" Current Brightness: ");
			Serial.println(current_brightness);
		}
#endif // DEBUG
		if (led_angle >= 0 * 360 / num_colors && led_angle < 1 * 360 / num_colors) {
			color_index = 0;
		}
		else if (led_angle >= 1 * 360 / num_colors && led_angle < 2 * 360 / num_colors) {
			color_index = 1;
		}
		else if (led_angle >= 2 * 360 / num_colors && led_angle < 3 * 360 / num_colors) {
			color_index = 2;
		}
		else if (led_angle >= 3 * 360 / num_colors && led_angle < 4 * 360 / num_colors) {
			color_index = 3;
		}
		else if (led_angle >= 4 * 360 / num_colors && led_angle < 5 * 360 / num_colors) {
			color_index = 4;
		}
		else {
			strip.setPixelColor(i, strip.Color(128, 128, 128));
		}
		r = round((float)colors[color_index].r * current_brightness);
		g = round((float)colors[color_index].g * current_brightness);
		b = round((float)colors[color_index].b * current_brightness);
		strip.setPixelColor(i, strip.Color(r, g, b));
	}
	strip.show();
}

void rainbow(uint8_t step) {
	uint16_t i;

	for (i = 0; i<strip.numPixels(); i++) {
		strip.setPixelColor(i, Wheel((i + step) & 255));
	}
	strip.show();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85) {
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
