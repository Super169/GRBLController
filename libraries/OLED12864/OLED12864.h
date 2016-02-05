#ifndef _OLED12864_H_
#define _OLED12864_H_

#include <avr/pgmspace.h>
#include "Arduino.h"

// #########################################################################################################

#define OLED_UNKNOWN		0
#define OLED_1306i2c 		1 		// for 0.96" OLED
#define	OLED_1106i2c 		2			// for 1.3" OLED

// #define _OLED12864_DEBUG_

#define BUFFER_SIZE 1024



// Comment out the following line if SRAM is enough, otherwise it will use Flash Memory as buffer
#define USE_FLASH_MEMORY

#ifndef USE_FLASH_MEMORY
	// disable PROGMEM if SRAM is used
	#define PROGMEM
#endif

// ############################


static	uint8_t  _invalidChar = 0x00;

// --------------------------------------------------------------------------------------------------------------

// It's not expected to change this setting
#define OLED_WIDTH 				128
#define OLED_HEIGHT 			64
#define OLED_LINE_HEIGHT	8
#define OLED_LINES				8


// Can be changed in runtime, or you can change the default if you only use one OLED
#define DEFAULT_I2C_ADDRESS			0x3C

// OLED Commands
#define OLED_CMD_DISPLAYON			0xAF
#define OLED_CMD_DISPLAYOFF			0xAE

//  Display Byte
//    0    E.g. D3 ->    1 \
//    1                  1  \ = 3 (0011)
//    2                  0  /
//    3                  0 /
//    4                  1 \
//    5                  0  \ = D (1101)
//    6                  1  /
//    7                  1 /

#define OLED_CMD_LOWCOLUMN   	0x00  // 0x00 - 0x0F
#define OLED_CMD_HIGHCOLUMN  	0x10  // 0x10 - 0x17
#define OLED_CMD_STARTLINE		0x40

#define OLED_CMD_SEGREMAP			0xA0

#define OLED_CMD_SCANNORMAL		0xC0
#define OLED_CMD_SCANREMAP		0xC8

#define OLED_CMD_COMPIN				0xDA

#define OLED_CMD_CONTRAST			0x81

#define OLED_CMD_RESUMEDISPLAY	0xA4

#define OLED_CMD_NORMALDISPLAY	0xA6
#define OLED_CMD_INCERSEDISPLAY	0xA7

#define OLED_CMD_1306_ADDRESSMODE 0x20   
// 10xb - Page addressing mode (PAGESTART, LOWCOLUMN, HIGHCOLUMN)
// 00xb - Horizontal addressing mode
// 01xb - Vertical addressing mode
#define OLED_HORIZONTAL_ADDRESSING			0x00    // 00xb
#define OLED_VERTICAL_ADDRESSING				0x01    // 01xb
#define OLED_PAGE_ADDRESSING						0x02    // 10xb

#define OLED_CMD_COLUMNADDRESS 		0x21
#define OLED_CMD_PAGEADDRESS   		0x22
#define OLED_CMD_STARTPAGE     		0xB0    // B0 - B7 (i.e. page start)


#define OLED_CMD_DISPLAYCLOCK			0xD5
#define OLED_CMD_MULTIPLEXRATIO		0xA8
#define OLED_CMD_DISPLAYOFFSET		0xD3

#define OLED_CMD_SETVCOMDETECT 		0xDB

#define OLED_CMD_1106_PUMPVOLTAGE	0x30
#define OLED_CMD_1106_DCCONTROL		0xAD
#define	OLED_CMD_1306_CHARGEPUMP	0x8D



#define swap(a, b) { int16_t t = a; a = b; b = t; }

#define OLED_fontUDF     00
#define OLED_font6x8     01
#define OLED_font8x16    02
#define OLED_fontNum     11
#define OLED_fontBigNum  12

#define OLED_SCROLL_UP		01
#define OLED_SCROLL_DOWN	02
#define OLED_SCROLL_LEFT	03
#define OLED_SCROLL_RIGHT	04

#define OLED_CMD_ACTIVATESCROLL 		0x2F
#define OLED_CMD_DEACTIVATESCROLL  	0x2E

#define OLED_MODE_DRAW		0
#define OLED_MODE_CLEAR		1
#define OLED_MODE_XOR 		2	

#define OLED_STYLE_EMPTY	0
#define OLED_STYLE_FILL		1



struct _DEFAULT_TEXT_MODE
{	
		boolean invertMode;
		uint8_t  drawType;
};

struct _FONT
{
	uint8_t code;
	boolean sram;
	const char *fontData;
	uint8_t width;
	uint8_t height;
	uint8_t lines;	// Based on height & OLED_LINE_HEIGHT, keep the value to reduce calculation during display
	uint8_t offset;
	uint8_t numChars;
	uint16_t bytePerChar;	
};

struct _CURSOR
{
	uint8_t x;
	uint8_t y;
};

struct _PLOTTER_SETUP
{
	boolean ready;
	boolean frame;
	boolean newDraw;
	int16_t xMin, xMax, xAxis;
	int16_t yMin, yMax, yAxis;
	uint8_t xMinPos, xMaxPos, xAxisPos;
	uint8_t yMinPos, yMaxPos, yAxisPos;
	int16_t xLast, yLast;
};



class OLED12864 {
	
	public:

		OLED12864(uint8_t chipset, boolean enableBuffer) {initialization(chipset, enableBuffer, true);}
		OLED12864(uint8_t chipset, boolean enableBuffer, boolean directDraw) {initialization(chipset, enableBuffer, directDraw);}
//		OLED12864(uint8_t chipset, boolean enableBuffer, boolean directDraw);
//		OLED12864(uint8_t chipset) { return OLED12864(chipset, false); }

		void sendCommand(uint8_t c);
		void sendData(uint8_t c);

		void begin();
		
// --------------------------------------------------------------------------------------------------------------

	
		void clr();
		void clr(uint8_t line, uint8_t start, uint8_t byte_cnt);
		void clr(uint8_t line_start, uint8_t line_end) { for (int r = line_start; r <= line_end; r++) {clr(r,0,OLED_WIDTH);} }
		void clr(uint8_t line) { return clr(line, 0, OLED_WIDTH); }
		
		void setInverse(boolean inverseMode);
		void setInverse() { return setInverse(true); }


		boolean invalidXY(uint16_t x, uint16_t  y) { return ((x < 0) || (x >= OLED_WIDTH) || (y < 0) || (y >= OLED_WIDTH)); }
		boolean invalidXL(uint16_t x, uint8_t line) { return ((x < 0) || (x >= OLED_WIDTH) || (line < 0) || (line >= OLED_LINES)); }
		boolean invalidLine(uint8_t line) { return ((line < 0) || (line >= OLED_LINES)); }
		
		
  // Must keep (y / 8) * OLED_WIDTH instead of y * 16 for 128 as it need the integer of (y / 8)
		uint16_t posXY(uint8_t x, uint8_t y) { return (x + (y / OLED_LINE_HEIGHT) * OLED_WIDTH); }
		uint16_t pos(uint8_t x, uint8_t y) { return posXY(x, y); }
		// Position by x and line
		uint16_t posXL(uint8_t x, uint8_t line) { return (x + line * OLED_WIDTH); }
		uint8_t pos2x(uint16_t pos) {return (pos % OLED_WIDTH);}
		uint8_t pos2y(uint16_t pos) {return (pos / OLED_WIDTH * OLED_LINE_HEIGHT); }
		uint8_t pos2line(uint16_t pos) {return (pos / OLED_WIDTH);}

		
		void setFont(uint8_t fontCode);
		void setFont(const char *font, boolean sram);
		
		void adjCursor();
		void setPrintPos(uint8_t x, uint8_t line);
		void println();
		
		void setInvalidChar(uint8_t c) { _invalidChar = c; }
		
		void print(uint8_t x, uint8_t line, const char ch);
		void print(uint8_t x, uint8_t line, const char ch[]);
		void print(const char ch[]) {return print(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), ch); }

		void printFlashMsg(uint8_t x, uint8_t line, const char* msgptr);
		void printlnFlashMsg(uint8_t x, uint8_t line, const char* msgptr) 
          {printFlashMsg(x, line, msgptr); println();}
		void printFlashMsg(const char* msgptr) 
          {printFlashMsg(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), msgptr);}
		void printlnFlashMsg(const char* msgptr) 
          {printFlashMsg(msgptr); println();}
    

		void printFlashMsgArr(uint8_t x, uint8_t line, const char* const* msg) 
          { return printFlashMsg(x, line, (char *) pgm_read_word(msg)); }
		void printlnFlashMsgArr(uint8_t x, uint8_t line, const char* const* msg) 
          { printFlashMsg(x, line, (char *) pgm_read_word(msg)); println(); }
		void printFlashMsgArr(const char* const* msg) 
          { printFlashMsg(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), (char *) pgm_read_word(msg));}
		void printlnFlashMsgArr(const char* const* msg) 
          { printFlashMsg(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), (char *) pgm_read_word(msg)); println();}

		void println(uint8_t x, uint8_t line, const char ch[]) { print(x, line, ch); println(); }
		void println(const char ch[]) { print(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), ch); println(); }
		
		// --- Numeric printing handling -------------------
		
		void printNum(uint8_t x, uint8_t line, long n, uint8_t base, uint8_t width, boolean fillZero);
		void printNum(uint8_t x, uint8_t line, long n, uint8_t base, uint8_t width) { return printNum(x, line, n, base, width, false); }
		void printNum(uint8_t x, uint8_t line, long n, uint8_t base) { return printNum(x, line, n, base, 0, false); }

		// All integer type go to long
		//
//		void print(uint8_t x, uint8_t line, int n, uint8_t width, boolean fillZero) {return printNum(x, line, n, 10, width, fillZero); }
//		void print(uint8_t x, uint8_t line, int n, uint8_t width) { return printNum(x, line, n, 10, width, false); }
//		void print(uint8_t x, uint8_t line, int n) { return printNum(x, line, n, 10, 0, false); }
//		void print(int n, uint8_t width, boolean fillZero) {return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 10, width, fillZero); }
//		void print(int n, uint8_t width) { return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 10, width, false); }
//		void print(int n) { return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 10, 0, false); }

		void print(uint8_t x, uint8_t line, long n, uint8_t width, boolean fillZero) {return printNum(x, line, n, 10, width, fillZero); }
		void print(uint8_t x, uint8_t line, long n, uint8_t width) { return printNum(x, line, n, 10, width, false); }
		void print(uint8_t x, uint8_t line, long n) { return printNum(x, line, n, 10, 0, false); }
		void print(long n, uint8_t width, boolean fillZero) {return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 10, width, fillZero); }
		void print(long n, uint8_t width) { return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 10, width, false); }
		void print(long n) { return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 10, 0, false); }
	
		void printHex(uint8_t x, uint8_t line, long n, uint8_t width, boolean fillZero) {return printNum(x, line, n, 16, width, fillZero); }
//		void printHex(uint8_t x, uint8_t line, long n, uint8_t width) { return printNum(x, line, n, 16, width, false); }
//		void printHex(uint8_t x, uint8_t line, long n) { return printNum(x, line, n, 16, 0, false); }
		void printHex(long n, uint8_t width, boolean fillZero) {return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 16, width, fillZero); }
		void printHex(long n, uint8_t width) { return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 16, width, false); }
		void printHex(long n) { return printNum(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 16, 0, false); }
	
		void printFloat(uint8_t x, uint8_t line, double n, uint8_t decimal, uint8_t width);
		void printFloat(uint8_t x, uint8_t line, double n, uint8_t decimal) { return printFloat(x, line, n, decimal, 0); }
		void printFloat(uint8_t x, uint8_t line, double n) { return printFloat(x, line, n, 2, 0); }
		void printFloat(double n, uint8_t decimal, uint8_t width) { return printFloat(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, decimal, width); }
		void printFloat(double n, uint8_t decimal) { return printFloat(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, decimal, 0); }
		void printFloat(double n) { return printFloat(_cursor.x, (_cursor.y / OLED_LINE_HEIGHT), n, 2, 0); }

		// ---------------------------------------------------------------------------------------------
		// Buffer related function 
		// For better compatibility only, include function and just do nothing if BUFFER is not ENABLED
		// ---------------------------------------------------------------------------------------------

		// Keep dummy function for better compatibility only, do nothing if BUFFER is not ENABLED
		void show();	
		void show(uint8_t line, uint8_t start, uint8_t byte_cnt);
		void show(uint8_t line) { return show(line, 0, OLED_WIDTH); }
		void showBuffer(uint16_t start_pos, uint16_t byte_cnt);
		// -- Screen scrolling
		void scrollLine(uint8_t direction, uint8_t lines);
		void scrollLine(uint8_t lines) { return scrollLine( OLED_SCROLL_UP, lines); }
		void scrollLine() { return scrollLine(OLED_SCROLL_UP, 1); }
		
		// ------------------------------------
		// Functions only available with buffer
		// ------------------------------------

		boolean setDirectDraw(boolean directDraw) { boolean b = _directDraw; _directDraw = directDraw; return b;}
		// -- Drawing functions 
		// Drawing mode:
		// 0 - Draw  // OR
		// 1 - Clear // AND NOT
		// 2 - XOR  		
		uint8_t drawBuffer(uint16_t bPos, uint8_t bits, uint8_t mode, uint8_t expBitCount);
		uint16_t drawBits(uint16_t bPos, uint8_t startBit, uint8_t endBit, uint8_t mode);
		uint16_t drawPixel(uint8_t x, uint8_t y, uint8_t mode) {return drawBuffer(pos(x,y), _BV((y%8)), mode, 1); }
		uint16_t drawPixel(uint8_t x, uint8_t y) {return drawBuffer(pos(x,y), _BV((y % 8)), 0, 1); }

		uint16_t drawVLine(uint8_t x, uint8_t y1, uint8_t y2, uint8_t mode);
		uint16_t drawHLine(uint8_t x1, uint8_t x2, uint8_t y, uint8_t mode);
		uint16_t drawHBits(uint8_t x1, uint8_t x2, uint8_t line, uint8_t drawPattern, uint8_t mode);
		uint16_t drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode, uint8_t style);

		// Plotter features

		boolean plotter(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax, int16_t xAxis, int16_t yAxis, boolean frame);
		boolean plotter(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax, int16_t xAxis, int16_t yAxis) { return plotter(xMin, xMax, yMin, yMax, xAxis, yAxis, false);}
		boolean plotter(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax) { return plotter(xMin, xMax, yMin, yMax, 0, 0, false);}

		void plotterClear();
		void plotterReset() { _plotter.newDraw = true; }
		boolean plotterDraw(int16_t x, int16_t y);
		boolean plotterDrawTo(int16_t x, int16_t y); 


	protected:

	private:

		uint8_t		_chipset;
		boolean   _inverseMode;
		boolean		_enableBuffer;
		boolean		_directDraw;
		boolean		_i2c;
		uint8_t		_i2cAddress;
		boolean		_externalVcc;
		uint8_t		_sda, _scl;

		uint8_t *_buffer;

		_FONT 		_font;
		_CURSOR		_cursor;
		boolean 	_lineScrolling;
		uint16_t	_scrollDelay;
		uint8_t 	_colOffset;				// To handle for _SH1106_, may need to find out a better fix

		_PLOTTER_SETUP _plotter;

		void initialization(uint8_t chipset, boolean enableBuffer, boolean directDraw);

		void setDisplayPos(uint8_t x, uint8_t line);

		uint8_t	getBuffer(uint16_t pos);
		
		uint16_t getFontPos(uint8_t c);
		uint8_t getFontData(uint16_t c, uint16_t address);
		
		void printError(uint8_t x, uint8_t y, uint8_t width, char errCode);
		void printError(uint8_t x, uint8_t y, uint8_t width) { return printError(x, y, width, '-');}

		uint16_t fillBuffer(uint8_t c, uint16_t startPos, uint16_t endPos);
		

		// Need to consider the converted positon outside current area
		int16_t getXPos(int16_t x);
		int16_t getYPos(int16_t y);
		void plotterDrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
		boolean plotterValidX(int16_t x) { return ((x >= _plotter.xMinPos) && (x <= _plotter.xMaxPos)); }
		boolean plotterValidY(int16_t y) { return ((y >= _plotter.yMaxPos) && (y <= _plotter.yMinPos)); }

		void i2cSendClr(uint16_t byte_cnt);
		void i2cSendBuffer(uint16_t start_pos, uint16_t byte_cnt);
};




#endif