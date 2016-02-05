#include "myKeyPad.h"

// modify this setting for your keypad if needed
#define KEY_PIN    0
#define KEY_COUNT  7
uint16_t adc_key[KEY_COUNT] = {200, 380, 480, 650, 800, 900, 960};

myKeyPad myKey(KEY_PIN, KEY_COUNT, adc_key);

#define KEY_LEFT   0
#define KEY_RIGHT  1
#define KEY_UP     5
#define KEY_DOWN   6
#define KEY_SELECT 3
#define KEY_CANCEL 4
#define KEY_COMBO  2

void setup() {
	Serial.begin(9600);

	myKey.debugShowKeys();
}

void loop() {
	switch(myKey.getKey()) {
		case KEY_LEFT:
			Serial.println("Left");
			break;
		case KEY_RIGHT:
			Serial.println("Right");
			break;
		case KEY_UP:
			Serial.println("Up");
			break;
		case KEY_DOWN:
			Serial.println("Down");
			break;
		case KEY_SELECT:
			Serial.println("Select");
			break;
		case KEY_CANCEL:
			Serial.println("Cancel");
			break;
		case KEY_COMBO:
			Serial.println("Combo");
			break;
		default:
			Serial.println("No key pressed");
			break;
	}
	delay(500);
}