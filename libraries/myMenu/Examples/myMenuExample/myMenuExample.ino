#define __PROG_TYPES_COMPAT__
#include <avr/pgmspace.h>

const char appName[] PROGMEM = "myMenu Example";

#include <Wire.h>
#include "OLED12864.h"

boolean enableBuffer = true;
boolean directDraw = true;

//  OLED_1306i2c - 0.96" OLED
//  OLED_1106i2c - 1.3" OLED
OLED12864 myOLED(OLED_1306i2c, enableBuffer, directDraw);

#include "myKeyPad.h"
// modify this setting for your keypad if needed
#define KEY_PIN    0
#define KEY_COUNT  7
uint16_t adc_key[KEY_COUNT] = {200, 380, 480, 650, 800, 900, 1020};

myKeyPad myKey(KEY_PIN, KEY_COUNT, adc_key);

#define KEY_LEFT   0
#define KEY_RIGHT  1
#define KEY_UP     5
#define KEY_DOWN   6
#define KEY_SELECT 3
#define KEY_CANCEL 4
#define KEY_COMBO  2

#include "myUtil.h"

#include "myMenu.h"
#include "menuSettings.ino"


myMenu appMenu(myOLED, 4, 7, false,
	             myKey, KEY_UP, KEY_DOWN, KEY_SELECT, KEY_CANCEL,
	             mainMenu, 5, true, '>');

char buffer[80];


void setup() {
	Serial.begin(57600);
	myUtil::getFlashMsg(buffer, appName, 80);
	Serial.println(buffer);

  myOLED.begin();
  myOLED.clr();
  myOLED.printFlashMsg(0,0, (char *) appName);

}


void loop()
{
	uint16_t menuSelection = 0;
	menuSelection = appMenu.getMenuSelection();
	Serial.print("Selected code: ");
	Serial.println(menuSelection);
	myOLED.clr(4,7);
	myOLED.printNum(0,4,menuSelection,10);

	// Do something for selection

	delay(1000);

}