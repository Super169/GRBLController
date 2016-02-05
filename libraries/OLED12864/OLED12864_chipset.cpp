//
//	OLED12864 - Chipset dependent methods
//
//
#include <Wire.h>
#include <avr/pgmspace.h>
#include <OLED12864.h>

void OLED12864::initialization(uint8_t chipset, boolean enableBuffer, boolean directDraw)
{
	switch (chipset)
	{
		case OLED_1306i2c:
			_colOffset = 0;
			break;
		case OLED_1106i2c:
			_colOffset = 2;
			break;
		default:
			_chipset = OLED_UNKNOWN;
			_enableBuffer = false;
			return;
	}
	_chipset = chipset;
	_i2c = true;
	_i2cAddress = DEFAULT_I2C_ADDRESS;
	if (enableBuffer) _buffer = (uint8_t *) malloc(BUFFER_SIZE);
	if (!_buffer) _enableBuffer = false;
	else _enableBuffer = true;

  _directDraw = ((!_enableBuffer) || directDraw);

	_plotter.ready = false;
}


void OLED12864::sendCommand(uint8_t c)
{
	if ((_i2c) && ((_chipset == OLED_1106i2c) || (_chipset == OLED_1306i2c))) 
	{
		uint8_t control = 0x00;   // Co = 0, D/C = 0
		Wire.beginTransmission(_i2cAddress);
		Wire.write(control);
		Wire.write(c);
		Wire.endTransmission();
	}
}

void OLED12864::sendData(uint8_t c)
{
	if ((_i2c) && ((_chipset == OLED_1106i2c) || (_chipset == OLED_1306i2c)))
	{
		uint8_t control = 0x40;   // Co = 0, D/C = 0
		Wire.beginTransmission(_i2cAddress);
		Wire.write(control);
		Wire.write(c);
		Wire.endTransmission();
	}
}

void OLED12864::begin()
{
	if (_i2c)
	{	
		Wire.begin();
		
		//  Sample initialization flow from SSD1306
		//		[AE],[A8,3F],[D3,00],[40],[A0/A1],[C0/C8],[DA,02],[81,7F],[A4],[A6],[D5,80],[8D,14],[AF]
		//
		//	Initialization from 1.3 OLED library for SSD1106 (I2C_with_Wire)
		//		[AE],[02],[10],[40],[B0],[81,80],[A1],[A6],[A8,3F],[AD,8B],[30],[C8],[D3,00],[D5,80],[D9,1F],[DA,12],[DB,40],[AF]
		//
		//  Initialization from AdaFruit SSD1306, 12864, with SSD1306_SWITCHCAPVCC (Adafruit_SSD1306)
		//    [AE],[D5,80],[A8,3F],[D3,00],[40],[8D,14],[20,00],[A1],[C8],[DA,12],[81,CF],[D9,22],[DB,40],[A4],[A6],[AF]
		//
		//  Initialization from 0.96 OLED software I2C (IIC_without_ACK_2)
		//		[AE],[00],[10],[40],[81,CF],[A1],[C8],[A6],[A8,3F],[D3,00],[D5,80],[D9,F1],[DA,12],[DB,40],[20,02],[8D,14],[A4],[A6],[AF]
		//
		//	Common Settings (exclude AE & AF which just turn OFF/ON the display):
		//		[A8,3F],[D3,00][40][A1][C8][A6][D5,80]
		//
		//	General Settings which not appear in all, but no harm to add, better add them for initialization
		//		[DA,12]				- Only the sample is set to 02
		//		[81,7F/80/CF] - Set contrast (set to CF for brighter level, change it if needed)
		//		[00][10]			- Set column to 0x00  (seems to be problem in SH1106, it has to set column to 0x02 (column #2)
		//		[B0]					- Set to page #0 (i.e. line 0)
		//		[A4]					- Entire Display ON
		//		[DB,40]				- Only missing in sample, for setting VCOM deselect level
		//
		//	Special Settings (need further study):
		//
		//		- Memory Addressing Mode [20] (only available for SSD1306)
		//				[20,00]		- Horizontal addressing mode (will advance to next line)
		//				[20,02]		- Will stop at end of line 
		//				Changing line is already handled inside the program, but [20,00] is good for screen redraw from buffer.
		//				So, [20,00] is used as default, need to consider the case when writing after lower-right, it will jump to upper-left.
		//
		//		- Charge Pump Setting
		//				SH1106:		[30]     (valid setting with command 30 - 33)	
		//				SSD1306:	[8D,14]
		//
		//		- DC-DC Control Mode	(only available for SH1106)
		//				[AD,8B]
		//
		//		- Pre=charge Period (default 2 DCLKS for both SH1106 & SSD1306, but the setting is different for all libraries)
		//
		//		Final initialization setting
		//			[AE],[A8,3F],[D3,00][40][A1][C8][A6][D5,80],[DA,12],[81,CF][00][10][B0][A4][DB,40][20,00]...[AF]
		//			- for SH1106:		[30][AD,8B]
		//			- for SSD1306:	[8D,14]
		//
	
		switch (_chipset)	
		{

			case OLED_1106i2c:

				sendCommand(OLED_CMD_DISPLAYOFF);                   // 0xAE (turn off the display during initialization
				
				sendCommand(OLED_CMD_MULTIPLEXRATIO);               // 0xA8
				sendCommand(0x3F);																	// Default 63 (0x3F)
				
				sendCommand(OLED_CMD_DISPLAYOFFSET);              	// 0xD3
				sendCommand(0x00);                                  // Reset offset to 0

				sendCommand(OLED_CMD_STARTLINE); 										// 40h 

				// Segment re-map is required in OLED
				sendCommand(OLED_CMD_SEGREMAP | 0x01);							// A1h	
				sendCommand(OLED_CMD_SCANREMAP);										// C8h	

				sendCommand(OLED_CMD_NORMALDISPLAY);								// A6h
				
				sendCommand(OLED_CMD_DISPLAYCLOCK);									// D5h
				sendCommand(0x80);																	// 0x80
				
				sendCommand(OLED_CMD_COMPIN);												// DAh
				sendCommand(0x12);																	// 0x12
				
				sendCommand(OLED_CMD_CONTRAST);											// 81h
				sendCommand(0xCF);																	// Default mid-level, change it if needed

				sendCommand(OLED_CMD_STARTPAGE ); 									// 0xB0 - page 0 (i.e. line 0)

				sendCommand(OLED_CMD_RESUMEDISPLAY);								// A4h
				
				sendCommand(OLED_CMD_SETVCOMDETECT);								// DBh
				sendCommand(0x40);																	// 0x40
				
				_colOffset = 0x02;																	// Column offset for SH1106 only
				sendCommand(OLED_CMD_LOWCOLUMN | _colOffset);  			// 0x00 - low col = 2
				sendCommand(OLED_CMD_HIGHCOLUMN );  								// 0x10 - hi col = 0
				sendCommand(OLED_CMD_1106_PUMPVOLTAGE);							// 30h
				sendCommand(OLED_CMD_1106_DCCONTROL);								// ADh
				sendCommand(0x8B);																	// 0x8B
				
				sendCommand(OLED_CMD_DEACTIVATESCROLL);							// Deactivate scrolling if any
				
				sendCommand(OLED_CMD_DISPLAYON);										// A6h (turn on the display)
				
				sendCommand(OLED_CMD_DISPLAYON);										// 0xAF

				break;


			case OLED_1306i2c:

				sendCommand(OLED_CMD_DISPLAYOFF);                   // 0xAE (turn off the display during initialization
				
				sendCommand(OLED_CMD_MULTIPLEXRATIO);               // 0xA8
				sendCommand(0x3F);																	// Default 63 (0x3F)
				
				sendCommand(OLED_CMD_DISPLAYOFFSET);              	// 0xD3
				sendCommand(0x00);                                  // Reset offset to 0

				sendCommand(OLED_CMD_STARTLINE); 										// 40h 

				// Segment re-map is required in OLED
				sendCommand(OLED_CMD_SEGREMAP | 0x01);							// A1h	
				sendCommand(OLED_CMD_SCANREMAP);										// C8h	

				sendCommand(OLED_CMD_NORMALDISPLAY);								// A6h
				
				sendCommand(OLED_CMD_DISPLAYCLOCK);									// D5h
				sendCommand(0x80);																	// 0x80
				
				sendCommand(OLED_CMD_COMPIN);												// DAh
				sendCommand(0x12);																	// 0x12
				
				sendCommand(OLED_CMD_CONTRAST);											// 81h
				sendCommand(0xCF);																	// Default mid-level, change it if needed

				sendCommand(OLED_CMD_STARTPAGE ); 									// 0xB0 - page 0 (i.e. line 0)

				sendCommand(OLED_CMD_RESUMEDISPLAY);								// A4h
				
				sendCommand(OLED_CMD_SETVCOMDETECT);								// DBh
				sendCommand(0x40);																	// 0x40
				
				_colOffset = 0;	
				sendCommand(OLED_CMD_1306_ADDRESSMODE);        		// 20h
				sendCommand(OLED_HORIZONTAL_ADDRESSING);          // 0x00 - Good for filling buffers
				sendCommand(OLED_CMD_LOWCOLUMN );  								// 0x00 - low col = 0
				sendCommand(OLED_CMD_HIGHCOLUMN );  							// 0x10 - hi col = 0
				sendCommand(OLED_CMD_1306_CHARGEPUMP);						// 8Dh
				sendCommand(0x14);																// 0x14
				
				sendCommand(OLED_CMD_DEACTIVATESCROLL);							// Deactivate scrolling if any
				
				sendCommand(OLED_CMD_DISPLAYON);										// A6h (turn on the display)
				
				sendCommand(OLED_CMD_DISPLAYON);										// 0xAF
				
				break;

		}
		setInverse(false);
		setFont(OLED_font6x8);															// default font
		_lineScrolling = true;															// enable line scrolling as default
		_scrollDelay = 50;																	// delay 50ms after scrolling one line
		
	}
}



// Update buffer only if enabled, otherwise direct draw on screen
void OLED12864::clr()
{ 
	byte fillData = 0x00;
	if (_inverseMode) fillData = 0xFF;
	_cursor.x = 0;
	_cursor.y = 0;
	if (_enableBuffer) fillBuffer(fillData, 0, BUFFER_SIZE - 1); 
	if (_directDraw)
	{
		if (_i2c)
		{
			switch (_chipset)	
			{

				case OLED_1106i2c:

					// direct clear screen
					for (uint8_t line = 0; line < OLED_LINES ; line++)
					{
						sendCommand(0xB0 + line); 												// line #0
						sendCommand(OLED_CMD_LOWCOLUMN | _colOffset);  		// low col = 2
						sendCommand(OLED_CMD_HIGHCOLUMN );  							// hi col = 0
					
						for (uint8_t x = 0; x < OLED_WIDTH; x++)
						{
							sendData(fillData);
						}
					}
					break;

				case OLED_1306i2c:

					// direct clear screen
					sendCommand(OLED_CMD_1306_ADDRESSMODE);          	// 0x20
					sendCommand(OLED_HORIZONTAL_ADDRESSING);         	// 0x00 - Good for filling buffers

					sendCommand(OLED_CMD_STARTPAGE ); 							 	// line #0
					sendCommand(OLED_CMD_LOWCOLUMN | _colOffset );  	// low col = 0
					sendCommand(OLED_CMD_HIGHCOLUMN );  							// hi col = 0

					for (uint16_t i = 0; i < BUFFER_SIZE; i += 8) 
					{
						Wire.beginTransmission(_i2cAddress);
						Wire.write(0x40);
						for (uint16_t x=0; x<8; x++) {
							Wire.write(fillData);
						}
						Wire.endTransmission();
					}
					break;

			}
		}	
	}
}


void OLED12864::clr(uint8_t line, uint8_t start, uint8_t byte_cnt)
{
	_cursor.x = start;
	_cursor.y = line * OLED_LINE_HEIGHT;
	byte col = start + _colOffset;

	byte fillData = 0x00;
	if (_inverseMode) fillData = 0xFF;

	if (_enableBuffer) fillBuffer(fillData, posXL(start, line), posXL(start, line) + byte_cnt); 
	if (_directDraw)
	{
		if (_i2c)
		{
			switch (_chipset)	
			{

				case OLED_1106i2c:


					// direct clear screen
					// sendCommand(OLED_CMD_STARTPAGE + line); 					// line #0
					// sendCommand(OLED_CMD_LOWCOLUMN | (col & 0xF));  	// low col = 2
					// sendCommand(OLED_CMD_HIGHCOLUMN | ((col & 0xF0) >> 4));  								// hi col = 0

					setDisplayPos(start, line);
					i2cSendClr(byte_cnt);
					break;

				case OLED_1306i2c:

					// direct clear screen
					sendCommand(OLED_CMD_1306_ADDRESSMODE);          	// 0x20
					sendCommand(OLED_HORIZONTAL_ADDRESSING);         	// 0x00 - Good for filling buffers

					// sendCommand(OLED_CMD_STARTPAGE + line); 							 	// line #0
					// sendCommand(OLED_CMD_LOWCOLUMN | _colOffset + start);  	// low col = 0
					// sendCommand(OLED_CMD_HIGHCOLUMN);  								// hi col = 0

					setDisplayPos(start, line);
					i2cSendClr(byte_cnt);
					break;

			}
		}	
	}
}


void OLED12864::show() 
{
	if (!_enableBuffer) return;

	if (_i2c)
	{

		switch (_chipset)	
		{

			case OLED_1106i2c:

				// Horizontal paging is not supported by SH1106, has to write to screen line by line
				for (uint8_t line = 0; line < OLED_LINES ; line++)
				{
					sendCommand(0xB0 + line); 												// line #0
					sendCommand(OLED_CMD_LOWCOLUMN | _colOffset);  		// low col = 2
					sendCommand(OLED_CMD_HIGHCOLUMN );  							// hi col = 0
				
					for (uint8_t x = 0; x < OLED_WIDTH; x++)
					{
						sendData(getBuffer(posXL(x,line)));
					}
				}
				break;

			case OLED_1306i2c:

				// Set the starting location to Page 0, Column 00 - i.e. upper left location
				sendCommand(OLED_CMD_1306_ADDRESSMODE);          	// 0x20
				sendCommand(OLED_HORIZONTAL_ADDRESSING);         	// 0x00 - Good for filling buffers

				sendCommand(OLED_CMD_STARTPAGE ); 							 	// line #0
				sendCommand(OLED_CMD_LOWCOLUMN | _colOffset );  	// low col = 0
				sendCommand(OLED_CMD_HIGHCOLUMN );  							// hi col = 0

				// Write 8 bytes for each transmission
				for (uint16_t i = 0; i < BUFFER_SIZE; i += 8) 
				{
				

					Wire.beginTransmission(_i2cAddress);
					Wire.write(0x40);
					for (uint16_t x = 0; x < 8; x++) {
					Wire.write(getBuffer(i + x));
					}
					Wire.endTransmission();
					
				}
				break;

		}

	}
}

void OLED12864::show(uint8_t line, uint8_t start, uint8_t byte_cnt)
{
	uint16_t start_pos = posXL(start, line);
	byte col = start + _colOffset;

	if (!_enableBuffer) return;

	if (_i2c)
	{
		switch (_chipset)	
		{

			case OLED_1106i2c:


				// direct clear screen
				// sendCommand(OLED_CMD_STARTPAGE + line); 					// line #0
				// sendCommand(OLED_CMD_LOWCOLUMN | (col & 0xF));  	// low col = 2
				// sendCommand(OLED_CMD_HIGHCOLUMN | ((col & 0xF0) >> 4));  								// hi col = 0

				setDisplayPos(start, line);
				i2cSendBuffer(start_pos, byte_cnt);
				break;

			case OLED_1306i2c:

				// direct clear screen
				sendCommand(OLED_CMD_1306_ADDRESSMODE);          	// 0x20
				sendCommand(OLED_HORIZONTAL_ADDRESSING);         	// 0x00 - Good for filling buffers

				// sendCommand(OLED_CMD_STARTPAGE + line); 							 	// line #0
				// sendCommand(OLED_CMD_LOWCOLUMN | _colOffset + start);  	// low col = 0
				// sendCommand(OLED_CMD_HIGHCOLUMN);  								// hi col = 0

				setDisplayPos(start, line);
				i2cSendBuffer(start_pos, byte_cnt);
				break;

		}
	}
}




void OLED12864::i2cSendClr(uint16_t byte_cnt) {
	uint16_t byte_send;
	byte fillData = 0x00;
	if (_inverseMode) fillData = 0xFF;

	for (uint16_t i = 0; i < byte_cnt; i += 30) 
	{
		// i2c can send 32 bytes, but one of it has been used for control code (0x40), so at most 31 byte can be send at one time
		// For safety, only send 30 at one time to speed up the drawing
		byte_send = min(30, byte_cnt - i);
		Wire.beginTransmission(_i2cAddress);
		Wire.write(0x40);
		for (uint16_t x=0; x < byte_send; x++) {
			Wire.write(fillData);
		}
		Wire.endTransmission();
	}
}

void OLED12864::i2cSendBuffer(uint16_t start_pos, uint16_t byte_cnt) {
	uint16_t byte_send;

	for (uint16_t i = 0; i < byte_cnt; i += 30) 
	{
		// i2c can send 32 bytes, but one of it has been used for control code (0x40), so at most 31 byte can be send at one time
		// For safety, only send 30 at one time to speed up the drawing
		byte_send = min(30, byte_cnt - i);
		Wire.beginTransmission(_i2cAddress);
		Wire.write(0x40);
		for (uint16_t x=0; x < byte_send; x++) {
			Wire.write(getBuffer(start_pos + i + x));
		}
		Wire.endTransmission();
	}
}

void OLED12864::setDisplayPos(uint8_t x, uint8_t line)
{
	if (invalidXL(x, line)) return;

	if (_i2c)
	{
		// With the setting of _colOffset, same code can be shared for SH1106 and SSD1306

		// Set the starting location to Page 0, Column 00 - i.e. upper left location
	  sendCommand(OLED_CMD_STARTPAGE  | line ); 										// line
		x += _colOffset;
		sendCommand(OLED_CMD_LOWCOLUMN  | ( x & 0x0f));  							// col low  = x & 0x0f
	  sendCommand(OLED_CMD_HIGHCOLUMN | (( x & 0xf0) >> 4));  			// col high = x & 0xf0
	}
}
