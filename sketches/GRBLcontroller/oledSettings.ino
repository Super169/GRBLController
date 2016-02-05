#ifndef _OLED_SETTIGNS_INC_
#define _OLED_SETTIGNS_INC_



// Only work with my OLED12864 library for 1106 and 1306 chips
#ifdef _OLED_ENABLED_
  #include "OLED12864.h"
  // Do not use drawing feature and disable buffering to save SRAM
  boolean enableBuffer = false;
  boolean directDraw = true;    // directDraw is always true as buffer disabled

	//  OLED_1306i2c - 0.96" OLED
	//  OLED_1106i2c - 1.3" OLED
	OLED12864 myOLED(OLED_1306i2c, enableBuffer, directDraw);
		
#endif

#endif  // _OLED_SETTIGNS_INC_