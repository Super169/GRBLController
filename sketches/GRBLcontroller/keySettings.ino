#ifndef _KEY_SETTIGNS_INC_
#define _KEY_SETTIGNS_INC_

#include "Arduino.h"

#if defined(_OLED_ENABLED_) && defined(_KEY_ENABLED_)

	#include "myKeyPad.h"
	
	// modify this setting for your keypad if needed
  // int adc_key[5] = {50,200,400,600,800};
  // int adc_key[5] = {350,600,750,850,980};
  #define KEY_PIN    0
  #define KEY_COUNT  8
  uint16_t adc_key[KEY_COUNT] = {200, 380, 480, 650, 800, 900, 960, 1020};

  myKeyPad myKey(KEY_PIN, KEY_COUNT, adc_key);

	#define KEY_LEFT   0
	#define KEY_RIGHT  1
	#define KEY_UP     5
	#define KEY_DOWN   6
	#define KEY_SELECT 3
	#define KEY_CANCEL 4
	#define KEY_COMBO  2
	#define KEY_DUMMY  7 // Dummy range  use to separate last key and no key


#endif  


#if !defined(_KEY_ENABLED_)
	#define START_PIN  2
#endif

#endif  // _KEY_SETTIGNS_INC_

