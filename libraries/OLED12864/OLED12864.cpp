#include <Wire.h>
#include <avr/pgmspace.h>
#include "OLED12864.h"
#include "oled12864font.c" 			// Change font file name to prevent overwrite from previous version


/*
		Pending Issues
			- Option for directDraw
			-	Public function for line drawing (already done in plotter, try to publish the function)
			- Split library by chipset inherit from OLED12864, some function may duplicated
			- Draw Text ... allow mix the text with backgroup drawing 
*/


void OLED12864::setInverse(boolean inverseMode) {
	_inverseMode = inverseMode;
}


void OLED12864::setFont(uint8_t fontCode)
{
	switch (fontCode)
	{
		case OLED_font6x8:
			_font.fontData = font6x8;	
			break;
		case OLED_font8x16:
			_font.fontData = font8x16;
			break;
		case OLED_fontNum:
			_font.fontData = fontNum;
			break;
		case OLED_fontBigNum:
			_font.fontData = fontBigNum;
			break;
		default:
			return;
	}
	_font.code				= fontCode;
	_font.sram				= false;
	_font.width				= pgm_read_byte(&_font.fontData[0]);
	_font.height			= pgm_read_byte(&_font.fontData[1]);
	_font.lines				= (_font.height + 7) / 8;
	_font.offset			= pgm_read_byte(&_font.fontData[2]);
	_font.numChars    = pgm_read_byte(&_font.fontData[3]);
	_font.bytePerChar = pgm_read_byte(&_font.fontData[4]);

	#ifdef _OLED12864_DEBUG_
		Serial.print("setFont: >> ");
		Serial.print(_font.code);
		Serial.print(" : ");
		Serial.print(_font.width);
		Serial.print(" : ");
		Serial.print(_font.height);
		Serial.print(" : ");
		Serial.print(_font.lines);
		Serial.print(" : ");
		Serial.print(_font.offset);
		Serial.print(" : ");
		Serial.print(_font.bytePerChar);
		Serial.println();
	#endif

	return;
}

void OLED12864::setFont(const char *font, boolean sram)
{
	_font.code = OLED_fontUDF;
	_font.sram = sram;
	_font.fontData = font;
	if (sram)
	{
		_font.width				= _font.fontData[0];
		_font.height			= _font.fontData[1];
		_font.lines				= (_font.height + 7) / 8;
		_font.offset			= _font.fontData[2];
		_font.numChars    = _font.fontData[3];
		_font.bytePerChar = _font.width * _font.height / 8;
	} else {
		_font.width				= pgm_read_byte(&_font.fontData[0]);
		_font.height			= pgm_read_byte(&_font.fontData[1]);
		_font.lines				= (_font.height + 7) / 8;
		_font.offset			= pgm_read_byte(&_font.fontData[2]);
		_font.numChars    = pgm_read_byte(&_font.fontData[3]);
		_font.bytePerChar = _font.width * _font.height / 8;
	}

	#ifdef _OLED12864_DEBUG_
		Serial.print("setFont: >> ");
		Serial.print(_font.code);
		Serial.print((_font.sram ? " : SRAM : " : " : PROGMEM : "));
		Serial.print(_font.width);
		Serial.print(" : ");
		Serial.print(_font.height);
		Serial.print(" : ");
		Serial.print(_font.lines);
		Serial.print(" : ");
		Serial.print(_font.offset);
		Serial.print(" : ");
		Serial.print(_font.bytePerChar);
		Serial.println();
	#endif
}


void OLED12864::setPrintPos(uint8_t x, uint8_t line)
{
	if (invalidXL(x, line)) return;
	setDisplayPos(x, line);
	_cursor.x = x;
	_cursor.y = OLED_LINE_HEIGHT * line;
}

void OLED12864::println()
{
	uint8_t lineScroll = 0;
	_cursor.x = 0;
	if (_cursor.y + 2 * _font.height > OLED_HEIGHT) 
	{
		scrollLine(OLED_SCROLL_UP, _font.lines);
	} 
	else
	{
		_cursor.y += _font.height;
	}
}

void OLED12864::printError(uint8_t x, uint8_t line, uint8_t width, char errCode)
{
	char buf[width + 1];
	for (int i = 0; i < width; i++) buf[i] = errCode;
	buf[width] = '\0';
	print(x,line,buf);
}

// Use long for maximum size
void OLED12864::printNum(uint8_t x, uint8_t line, long n, uint8_t base, uint8_t width, boolean fillZero) {
	#ifdef _OLED12864_DEBUG_
		Serial.print("OLED12864::printNumber>>  ");
		Serial.print(x);
		Serial.print(" : ");
		Serial.print(line);
		Serial.print(" : ");
		Serial.print(n);
		Serial.print(" : ");
		Serial.print(base);
		Serial.print(" : ");
		Serial.print(width);
		Serial.print(" : ");
		Serial.print(fillZero);
		Serial.println();
	#endif

  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];
	
	if (isnan(n)) return printError(x, line, width, 'N');
	if (isinf(n)) return printError(x, line, width, 'I');

	// Invalid width
	if (width > sizeof(buf)) 
	{
		printError(x,line,sizeof(buf));
		return;
	}
	
	boolean nve = (n < 0);
	if (nve) n = -n;	
	
	if (width < 1) width = 0;
	int w = 0;
	
  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
		// Continue to build the string
		// if (++w == width) break ;
		w++;
  } while(n);

	if (width && ((w > width) || (nve && (w == width))))
	{
		if (nve) *--str = '-';
		#ifdef _OLED12864_DEBUG_
			Serial.print(" : Over width : ");
			Serial.println(str);
		#endif
		return printError(x, line, width, '*');
	}

	// Different cases
	// - : fill zero after '-', fill space before '-'
	// + : simple, just add leading characters
	
	if (nve)
	{
		if (fillZero) {
			while (w < width - 1)
			{
				*--str = '0';
				w++;
			}
			*--str = '-';
			w++;
		} else {
			*--str = '-';
			w++;
			while (w < width)
			{
				*--str = ' ';
				w++;
			}
		}
	} 
	else 
	{
		while (w < width)
		{
			*--str = ((fillZero && !nve) ? '0' : ' ');
			w++;
		}
	}
	
	#ifdef _OLED12864_DEBUG_
		Serial.print(" : normal >> ");
		Serial.println(str);
	#endif
	
  print(x, line, str);	
}


// For floating number, leading zero is not supported, overall width is used
void OLED12864::printFloat(uint8_t x, uint8_t line, double n, uint8_t decimal, uint8_t width)
{
	#ifdef _OLED12864_DEBUG_
		Serial.print("OLED12864::printFloat>> ");
		Serial.print(n);
		Serial.print(" : ");
		Serial.print(decimal);
		Serial.print(" : ");
		Serial.print(width);
		Serial.println();
	#endif

  char buf[21]; // Enforce to max. 20 characters, which should be good enough
  char *str = &buf[sizeof(buf) - 1];
	if (isnan(n)) return printError(x, line, width, 'N');
	if (isinf(n)) return printError(x, line, width, 'I');
	if (n > 4294967040.0) return printError(x, line, width, '^');
	if (n < -4294967040.0) return printError(x, line, width, 'v');


	// Invalid width
	if (width > sizeof(buf)) 
	{
		printError(x,line,sizeof(buf));
		return;
	}

	// Display as integer if decimal <= 0
	if (decimal <= 0) return printNum(x, line, (int) n, 10, width, false);
	
	if (width < 0) width = 0;
	if (width > 20) width = 20; 
	if (width && (decimal > width - 1))
	{
		printError(x, line, width);
		return;
	}

	boolean nve = (n < 0);
	if (nve) n = -n;
	
	long int_part1 = (long) n;
	double reminder = n - int_part1;
	
	for (int i = 0; i < decimal; i++) reminder *= 10;
	long int_part2 = (long) reminder;
	reminder -= int_part2;
	if (reminder > 0.5) int_part2 += 1;
	
	#ifdef _OLED12864_DEBUG_
		Serial.print(" : ");
		Serial.print(int_part1);
		Serial.print(" : ");
		Serial.print(reminder);
		Serial.print(" : ");
		Serial.print(int_part2);
	#endif
	
	*str = '\0';
	int w = 0;
	
	for (int i = 0; i < decimal; i++)
	{
    unsigned long m = int_part2;
    int_part2 /= 10;
    char c = m - int_part2 * 10;
    *--str = c + '0';
		w++;
	}
	*--str = '.';
	w++;
	
	// Special handle for 0.X which can be displayed as .XX only
	if ((!nve) && (w == width) && (int_part1 == 0)) 
	{
		#ifdef _OLED12864_DEBUG_
			Serial.println(" : 0.X");
		#endif
		return print(x, line, str);
	}
	
	do {
		unsigned long m = int_part1;
		int_part1 /= 10;
		char c = m - 10 * int_part1;
		*--str = c + '0';
		// Continue to build the string
		// if (++w == width) break ;
		w++;
	} while(int_part1);

	if (nve) 
	{	
		*--str = '-';
		w++;
	}
	
	if ((width > 0) && (w > width)) 
	{
		#ifdef _OLED12864_DEBUG_
			Serial.print(" : Over width > ");
			Serial.println(str);
		#endif
		return printError(x, line, width, '*');
	}
	
	while (w < width)
	{
		*--str = ' ';
		w++;
	}	
	
	#ifdef _OLED12864_DEBUG_
		Serial.print(" : normal >> ");
		Serial.println(str);
	#endif
	return print(x,line,str);
}

// ---------------------------------------------------------------------------------------------------

void OLED12864::adjCursor()
{
	#ifdef _OLED12864_DEBUG_
		Serial.print("adjCursor: scroll >> ");
		Serial.print(_cursor.x);
		Serial.print(" : ");
		Serial.print(_cursor.y);
	#endif

	if (_cursor.x + _font.width > OLED_WIDTH) {
		// No more character can be printed
		_cursor.x = 0;
		_cursor.y += _font.height;
	}
	
	if (_cursor.y + _font.height > OLED_HEIGHT) {
		int minLineScroll = (_cursor.y + _font.height - OLED_HEIGHT - 1) / OLED_LINE_HEIGHT + 1;
		scrollLine(OLED_SCROLL_UP, minLineScroll);
		_cursor.y -= (minLineScroll * OLED_LINE_HEIGHT);
		#ifdef _OLED12864_DEBUG_
			Serial.print(" : ");
			Serial.print(minLineScroll);
			Serial.print(" : ");
			Serial.print(_cursor.y);
		#endif
		show();		// MUST update the display after scroll
		if (_scrollDelay) delay(_scrollDelay);  
	} 
	#ifdef _OLED12864_DEBUG_
		Serial.println();
	#endif
}

void OLED12864::print(uint8_t x, uint8_t line, const char ch) {
	// Use string mode for printing
	char printStr[] = " ";
	printStr[0] = ch;
	print(x, line, printStr);
}

void OLED12864::print(uint8_t x, uint8_t line, const char ch[])
{

	// simplified for testing only 
	// if (invalidXL(x, line)) return;
	// -- This will introduce a bug if last printing stopped at last position which will not advance the line until next print.

	// Try just valid the line only
	if (invalidLine(line)) return;	

	uint8_t j;
	uint16_t c;
	uint16_t bPos;
	uint8_t data;
	
	_cursor.x = x;
	_cursor.y = line * OLED_LINE_HEIGHT;
	
	int maxChars, charsWrite;

	j = 0;
	while (ch[j] != '\0')
	{
		adjCursor();
		maxChars = ((OLED_WIDTH - _cursor.x) / _font.width); 	// number of characters can be printed in this line.
		charsWrite = 0;
		if (maxChars > 0)
		{
			line = (_cursor.y / OLED_LINE_HEIGHT);
			for (int k = 0; k < _font.lines; k++) 
			{
				setDisplayPos(_cursor.x, line + k);
				bPos = posXL(_cursor.x, line + k);
				for (int l = 0; (( l < maxChars) && (ch[j + l] != '\0')) ; l++)
				{
					c =ch[j+l] - _font.offset;

					if (_font.width <= 24) {
						if (_directDraw) Wire.beginTransmission(_i2cAddress);
						if (_directDraw) Wire.write(0x40);
						for (uint8_t i=0; i < _font.width; i++) 
						{
							data = getFontData(c, 5 + c * _font.bytePerChar + k * _font.width + i);
							if (_directDraw) Wire.write(data);
							if (_enableBuffer)
								_buffer[bPos++] = data;
						}
						if (_directDraw) Wire.endTransmission();
					} else {
						for (uint8_t i=0; i < _font.width; i += 8) 
						{
							if (_directDraw) Wire.beginTransmission(_i2cAddress);
							if (_directDraw) Wire.write(0x40);
							uint8_t maxX = min(8, _font.width - i);
							for (uint8_t x=0; x < maxX; x++) {
								data = getFontData(c, 5 + c * _font.bytePerChar + k * _font.width + i + x);
								if (_directDraw) Wire.write(data);
								if (_enableBuffer)
									_buffer[bPos++] = data;
							}
							if (_directDraw) Wire.endTransmission();
						}
					}

					if (k == 0) charsWrite++;  // only count on first row of chars; it may use l as charsWrite, but will need to check stop condition
				}
			}
		}
		j += charsWrite;
		_cursor.x += charsWrite * _font.width;
		if (ch[j] != '\0')
		{
			// advance to next line
			_cursor.x = 0;
			_cursor.y += _font.height;
		}
	}		
}


void OLED12864::printFlashMsg(uint8_t x, uint8_t line, const char* msgptr)
{
	#ifdef _OLED12864_DEBUG_
		Serial.println("printFlashMsg");
	#endif

	_cursor.x = x;
	_cursor.y = line * OLED_LINE_HEIGHT;	

	// print at most 8 characters at a time (take the balance between buffer size and speed)
	char prtBuffer[9];

	uint8_t idx = 0;
	uint16_t ptr = 0;
	boolean goPrint = true;

	memset(prtBuffer, 0x00, 9);
	do {

//	  Serial.print(ptr);
//	  Serial.print(" : ");
//	  Serial.println(pgm_read_byte(&msgptr[ptr]));
		
		goPrint = (prtBuffer[idx++]	= pgm_read_byte(&msgptr[ptr++]));
		if (goPrint) {
			if (idx == 8) {
				print(prtBuffer);
				#ifdef _OLED12864_DEBUG_
					Serial.print(prtBuffer);
				#endif
				memset(prtBuffer, 0x00, 8);
				idx = 0;
			}
		} else {
			if (idx > 1) print(prtBuffer);
		}
	} while (goPrint);
	#ifdef _OLED12864_DEBUG_
		Serial.println();
	#endif

}

/*

void OLED12864::printFlashMsgArr(uint8_t x, uint8_t line, const char** msg)
{

  Serial.println("printFlashMsgArr");
	Serial.print(x);
  Serial.print(" ,");
	Serial.print(line);
	Serial.println();
  
  printFlashMsg(x, line, (char *)pgm_read_word(msg));

  char* msgptr = (char *)pgm_read_word(msg);

	_cursor.x = x;
	_cursor.y = line * OLED_LINE_HEIGHT;	

	// print at most 8 characters at a time (take the balance between buffer size and speed)
	char prtBuffer[9];

	uint8_t idx = 0;
	uint16_t ptr = 0;
	boolean goPrint = true;

	memset(prtBuffer, 0x00, 9);
	do {
	  Serial.print(ptr);
	  Serial.print(" : ");
	  Serial.println(pgm_read_byte(&msgptr[ptr]));
		goPrint = (prtBuffer[idx++]	= pgm_read_byte(&msgptr[ptr++]));
		if (goPrint) {
			if (idx == 8) {
				print(prtBuffer);
				memset(prtBuffer, 0x00, 8);
				idx = 0;
			}
		} else {
			if (idx > 1) print(prtBuffer);
		}
	} while (goPrint);
	
}

*/

uint8_t OLED12864::getFontData(uint16_t c, uint16_t address)
{
	uint8_t data;
	if ((c < 0) or (c >= _font.numChars)) return _invalidChar;
	if (_font.sram) data = _font.fontData[address];
	else data = pgm_read_byte(&_font.fontData[address]);
	if (_inverseMode) data = ~data;
	return data;
}