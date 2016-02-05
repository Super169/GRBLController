/*
  20151103 - Add optional parameter padChar for f2s

*/

#ifndef _myUtil_H_
#define _myUtil_H_

#include "Arduino.h"
/*
#include "SuperUI.h"
#include "SuperUI_HWSerial.h"
#include <SPI.h>
#include "UTFT.h"
#include "SuperUI_UTFT9225.h"
*/

#define _F2S_PAD_NONE  0
#define _F2S_PAD_LEFT  1
#define _F2S_PAD_RIGHT 2

class myUtil {

	public:

		static unsigned int availableMemory(unsigned int max);
		static void myDelay(unsigned long idle_period);
		static uint8_t f2s(char* buffer, double data, uint8_t decimal, uint8_t width, uint8_t padMode, char padChar);
		static uint8_t f2s(char* buffer, double data, uint8_t decimal, uint8_t width, uint8_t padMode) { return f2s(buffer, data, decimal, width, padMode, ' '); }
		static double adjAverage(double* samples, uint8_t sample_cnt);
    static uint8_t getFlashMsg(char * localMsgPtr, const char * flashMsgPtr, uint8_t buffer_size);
	
	private:		
		static double getFactor(uint8_t len);
		static uint8_t fillError(char* buffer, uint8_t width);
		static uint8_t setNum(char* buffer, uint8_t idx, unsigned long data, uint8_t len);
		static uint8_t getLen(unsigned long data);
};

#endif