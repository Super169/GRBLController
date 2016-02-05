#include <Wire.h>
#include <SPI.h>
#include <SdFat.h>
#include "myUtil.h"
#include "myGRBL.h"

const char msg_appname[] PROGMEM = "GRBL Controller v1.0";

//  --------------------------------------
//  | _MEGA_ENABLED_ | Serial  | Serial2 |
//  |----------------+---------+---------|
//  | Defined        | DEBUG   | GRBL    |
//  | Not Define     | GRBL    | N/A     |
//  --------------------------------------
//  ---------------------------------------------------------------------------------------------------------
//  | _OLED_ENABLED_ | _KEY_ENABLED_  | Behaviors                                                           |
//  |----------------+----------------+---------------------------------------------------------------------|
//  | Defined        | Defined        | Full function, follow selection in OLED, show inform in OLED        |
//  | Defined        | Not Defined    | Try to send auto.gcode from SD when D3 is LOW, show inform in OLED. |
//  | Not Define     | N/A (Ignored)  | Try to send auto.gcode from SD when D3 is LOW.                      |
//  ---------------------------------------------------------------------------------------------------------
//
//  **** _KEY_ENABLED_ must be used with _OLED_ENABLED_ for controller UI

// #define _MEGA_ENABLED_
#define _OLED_ENABLED_
#define _KEY_ENABLED_

#include "appSettings.ino"

#define FILENAME_SIZE 48
#define FILEIDX_SIZE 16
#define LINE_NUMBER_SIZE 6

// Buffer for user input.
#define INBUFFER_SIZE 80

// Multi-purpose buffer
char inBuffer[INBUFFER_SIZE+1];

// For safety, use different buffer for message, if needed, may change to use inBuffer, but need to take care of different usage
#define MSGBUFFER_SIZE 80
char msgBuffer[MSGBUFFER_SIZE+1];

unsigned long line_number = 0;
char sLine[LINE_NUMBER_SIZE + 1];

char fileName[FILENAME_SIZE] = "auto.gcode";
int fileIdx[FILEIDX_SIZE];
int fileCnt = 0;
int filePtr = -1;
boolean fileSelected = false;

// SD chip select pin
const uint8_t chipSelect = SS;
boolean validFile = false;

// file system object
SdFat sd;
SdFile file;

void setup() {

	#ifdef _MEGA_ENABLED_  
	  Serial.begin(DEBUG_SERIAL_BAUD);
	  printlnFlashMsg(msg_appname);
	#endif

	#ifdef _OLED_ENABLED_
	  myOLED.begin();
	  myOLED.clr();
//	  myOLED.show();
	  myOLED.setFont(OLED_font6x8);
	  myOLED.printFlashMsg(0,0,msg_appname);
	  myOLED.print(96, 1, (long) myUtil::availableMemory(8192));
//	  myOLED.show();

	#endif

	pinMode(GRBL_RESET_PIN, OUTPUT);
	digitalWrite(3, HIGH);
	
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  // It's too bad that sdFat library will sent error to Serail, 
  // so CNC connection MUST be started after SD checking to avoid sending message to GRBL
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
  	#ifdef _OLED_ENABLED_
  		myOLED.clr(2);
		  myOLED.printFlashMsg(0,2,msg_sd_failed);
  	#endif 
  	#ifdef _MEGA_ENABLED_
		  printlnFlashMsg(msg_sd_failed);
  	#endif
    sd.initErrorHalt();
  }

  // Check application mode, if no OLED or no KEYPAD, go to send auto.gcode directly
  #if !defined(_OLED_ENABLED_) || !defined(_KEY_ENABLED_)
  	// KeyPad not available, just send file auto.gcode
  	
  	pinMode(START_PIN, INPUT_PULLUP);
  	#ifdef _OLED_ENABLED_
  		myOLED.clr(2);
		  myOLED.printFlashMsg(0,2,msg_wait_gnd);
  	#endif
  	#ifdef _MEGA_ENABLED_
		  printlnFlashMsg(msg_wait_gnd);
  	#endif
  	while (digitalRead(START_PIN) != LOW);
  	#ifdef _OLED_ENABLED_
  		myOLED.clr(2);
  	#endif

  	if (!connectGRBL()) while (1);  // halt if fail connecting to GRBL
  	sendFile();

  	while (1);

  #else
  	
	  readSdRoot();


  #endif

}


void loop() {
	
	#if defined(_OLED_ENABLED_) && defined(_KEY_ENABLED_)

		// Must have OLED and KeyPad
		uint16_t menuSelection = 0;
		menuSelection = appMenu.getMenuSelection();
		appMenu.clr();
		switch (menuSelection) {
			case MENU_CONNECT_CNC:
				goConnectCNC();
				break;
			case MENU_JOG_CONTINUE:
				goJogMode(false);
				break;
			case MENU_JOG_SINGLE:
				goJogMode(true);
				break;
			case MENU_CHECK_STATUS:
    		showGrblReturn(cnc.send("?", 1), 3);
				waitSelectCancel(7);
        break;
			case MENU_SEND_FILE:
				goSendFile();
				break;
			case MENU_SOFT_RESET:
				showGrblReturn(cnc.softReset(), 4);
				waitSelectCancel(6);
				break;
			case MENU_HARD_RESET:
				goHardReset();
				break;
			case MENU_GO_HOME:
    		showGrblReturn(cnc.send("G28", 1), 4);
				waitSelect(6);
				break;
			case MENU_SET_000:
    		showGrblReturn(cnc.send("G92 X0 Y0 Z0", 1), 4);
				waitSelect(6);
				break;
			case MENU_GO_000:
    		showGrblReturn(cnc.send("G90 G1 X0 Y0 Z0 F100", 1), 4);
				waitSelect(6);
				break;
			case MENU_PROBE_Z:
        // myOLED.print(0, 4, msg_probe_Z);
				if (getSelectCancel(4, msg_probe_Z, 6)) {
					myOLED.clr(4,7);
	    		showGrblReturn(cnc.send("G91 G21", 1), 4);
	    		showGrblReturn(cnc.send("G38.2 Z-30 F75", 1), 4);
	    		showGrblReturn(cnc.send("G0 Z1", 1), 4);
	    		showGrblReturn(cnc.send("G38.2 Z-1 F7.5", 1), 4);
					waitSelect(7);
				}
				break;
			case MENU_SET_Z0:
    		showGrblReturn(cnc.send("G92 Z0", 1), 3);
				waitSelect(7);
				break;
		}
  #endif
}


#if defined(_OLED_ENABLED_) && defined(_KEY_ENABLED_)

void goConnectCNC() {
	if (connectGRBL()) {
		waitSelectCancel(4, msg_grbl_connected, 6);
  } else {
		waitSelectCancel(4, msg_grbl_connect_fail, 6);
	}
}

void goJogMode(boolean jogSingle) {
	if (!cnc.isReady()) {
		if (!connectGRBL()) {
			waitSelect(4, msg_grbl_connect_fail, 6);
			return;
  	}
	}
/*
	if (jogSingle) {
	  myOLED.printFlashMsg(0,3,msg_jog_single);
	} else {
	  myOLED.printFlashMsg(0,3,msg_jog_continue);
	}
*/	
  myOLED.printFlashMsg(0,3,(jogSingle ? msg_jog_single : msg_jog_continue));
  boolean goJog = true;
  uint8_t key;
  int8_t jogAxis = 0;
  showJogAxis(jogAxis);
//  int retCode;
//	char GRBL_JOG_STR[] = "G91 G1 X+0.1 F100";

  while (goJog) {
    key = myKey.getKey(jogSingle);
    switch (key) {
    	case KEY_UP:
    		/*
        GRBL_JOG_STR[7] = 'X' + jogAxis;
        GRBL_JOG_STR[8] = '+';
        myOLED.print(0,4,GRBL_JOG_STR);
    		showGrblReturn(retCode = cnc.send(GRBL_JOG_STR, 1), 5);
    		delay(10);
    		*/
    		goJogCNC(jogAxis, '+');
    	  break;
    	case KEY_DOWN:
    		/*
        GRBL_JOG_STR[7] = 'X' + jogAxis;
        GRBL_JOG_STR[8] = '-';
        myOLED.print(0,4,GRBL_JOG_STR);
    		showGrblReturn(retCode = cnc.send(GRBL_JOG_STR, 1), 5);
    		delay(10);
    		*/
    		goJogCNC(jogAxis, '-');
    	  break;
    	case KEY_LEFT:
    	  jogAxis = (jogAxis == 0 ? 2 : jogAxis - 1);
        while (myKey.getKey(false) != -1);
        showJogAxis(jogAxis);
    	  break;
    	case KEY_RIGHT:
    	  jogAxis = (jogAxis == 2 ? 0 : jogAxis + 1);
        showJogAxis(jogAxis);
        while (myKey.getKey(false) != -1);
    	  break;
    	case KEY_SELECT:
    		// showGrblReturn(retCode = cnc.send("?", 1), 4);
    		showGrblReturn(cnc.send("?", 1), 4);
        break;
    	case KEY_CANCEL:
    	  goJog = false;
        break;
    }
  }
}

void goJogCNC(uint8_t jogAxis, char sign) {
	char GRBL_JOG_STR[] = "G91 G1 X+0.1 F100";
  GRBL_JOG_STR[7] = 'X' + jogAxis;
  GRBL_JOG_STR[8] = sign;
  myOLED.print(0,4,GRBL_JOG_STR);
	showGrblReturn(cnc.send(GRBL_JOG_STR, 1), 5);
	delay(10);
}

void showJogAxis(uint8_t jogAxis) {
  char jogAxisName = 'X' + jogAxis;
  myOLED.print(0,3,jogAxisName);
}

void goHardReset() {
	myOLED.printFlashMsg(0, 4, msg_cnc_hard_reset);
	digitalWrite(GRBL_RESET_PIN, LOW);
	delay(500);
	digitalWrite(GRBL_RESET_PIN, HIGH);
	delay(500);
	// Reconnect for safety
	goConnectCNC();
}

void goSendFile() {

  if (fileCnt == 0) {
		#ifdef _MEGA_ENABLED_
			printFlashMsg(msg_sd_empty);
		#endif
	  waitSelect(0, msg_sd_empty, 5);
		return;
  }

	fileSelected = false;
	if (!selectFile()) {
		myOLED.clr(2,7);
		return;
	}

	myOLED.clr(2,7);
	if (!cnc.isReady()) {
		if (!connectGRBL()) {
			waitSelect(4, msg_grbl_connect_fail, 6);
			return;
  	}
	}

	sendFile();

  #ifdef _MEGA_ENABLED_
  	printlnFlashMsg(msg_ui_complete);
  #endif
  waitSelectCancel(3, msg_ui_complete, 5);
	myOLED.clr(2,7);
}


void waitSelect(int waitRow) {
	showMsgWaitReply(-1, 0, waitRow, msg_key_wait_select, 1);
}

boolean waitSelectCancel(int displayRow) {
	uint8_t keys[2] = {KEY_SELECT, KEY_CANCEL};
	myOLED.printFlashMsg(0, displayRow, msg_key_wait_select_cancel);
	uint8_t key = myKey.waitKeys(keys, 2, true);
	return (key == KEY_SELECT);
}


void waitSelect(int msgRow, const char* msgptr, int waitRow) {
	showMsgWaitReply(msgRow, msgptr, waitRow, msg_key_wait_select, 1);
}

void waitSelectCancel(int msgRow, const char* msgptr, int waitRow) {
	showMsgWaitReply(msgRow, msgptr, waitRow, msg_key_wait_select_cancel, 2);
}

boolean getSelectCancel(int msgRow, const char* msgptr, int waitRow) {
	return showMsgWaitReply(msgRow, msgptr, waitRow, msg_key_get_select_cancel, 2);
}

boolean showMsgWaitReply(int msgRow, const char* msgptr, int waitRow, const char* waitMsgptr, uint8_t keyCnt) {
	uint8_t waitKeys[2] = {KEY_SELECT, KEY_CANCEL};
	myOLED.clr(msgRow, waitRow);
	if (msgRow >= 0) myOLED.printFlashMsg(0,msgRow, msgptr);
	myOLED.printFlashMsg(0, waitRow, waitMsgptr);
	return (myKey.waitKeys(waitKeys, keyCnt) == KEY_SELECT);
}

#endif


boolean connectGRBL() {
  // Connect to GRBL and start and send a soft reset
	#ifdef _OLED_ENABLED_
		myOLED.clr(1);
		myOLED.printFlashMsg(0,4,msg_grbl_go_connect);
	#endif
	#ifdef _MEGA_ENABLED_  
		printlnFlashMsg(msg_grbl_go_connect);
	#endif	


	cnc.setTimeout(0);
	int r;
	r = cnc.begin();

	if (!cnc.isReady()) {
		#ifdef _MEGA_ENABLED_  
			printlnFlashMsg(msg_grbl_erro_reset);
			showGrblReturn(r);
			printlnFlashMsg(msg_grbl_erro_reset);
		#endif	
		return false;
	};

	// CNC ready
	#ifdef _MEGA_ENABLED_
		Serial.print(cnc.getBuffer());
	#endif

	// Getting GRBL information for MEGA board only 
	#if defined(_MEGA_ENABLED_) || defined(_OLED_ENABLED_)
		if (cnc.send("$I", 1) < GRBL_RETURN_ERROR) {
			#ifdef _MEGA_ENABLED_
				printFlashMsg(msg_grbl_build_info);
				Serial.print(cnc.getBuffer());
			#endif
			#ifdef _OLED_ENABLED_
				myOLED.print(0,1,cnc.getBuffer());
			#endif
		}
	#endif

	return true;
}

// void showGrblReturn(int retCode) { showGrblReturn(retCode, 4); }


void showGrblReturn(int retCode, int startRow) {
	#ifdef _OLED_ENABLED_
		myOLED.clr(5,7);
	#endif
	switch (retCode) {
	case GRBL_RETURN_NOTREADY:
		#ifdef _MEGA_ENABLED_
			printlnFlashMsg(msg_grbl_not_ready);
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.printFlashMsg(0,startRow,msg_grbl_not_ready);
		#endif	
		break;
	case GRBL_RETURN_OK:
		#ifdef _MEGA_ENABLED_
			printlnFlashMsg(msg_grbl_ok);
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.printFlashMsg(0,startRow,msg_grbl_ok);
		#endif	
		break;
	case GRBL_RETURN_GRBL:
		#ifdef _MEGA_ENABLED_
			printlnFlashMsg(msg_grbl_reset);
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.printFlashMsg(0,startRow,msg_grbl_reset);
		#endif	
		break;
	case GRBL_RETURN_END_OK:
	case GRBL_RETURN_OTHERS:
		#ifdef _MEGA_ENABLED_
			Serial.print(">> ");
			Serial.println(cnc.getBuffer());
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.print(0,startRow,cnc.getBuffer());
		#endif	
		break;
	case GRBL_RETURN_ERROR:
		#ifdef _MEGA_ENABLED_
			Serial.print("E> ");
			Serial.println(cnc.getBuffer());
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.print(0,startRow,cnc.getBuffer());
		#endif	
		break;
	case GRBL_RETURN_NO_CONNECTION:
		#ifdef _MEGA_ENABLED_
			printlnFlashMsg(msg_grbl_no_connection);
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.printFlashMsg(0,startRow,msg_grbl_no_connection);
		#endif	
		break;
	case GRBL_RETURN_TIMEOUT:
		#ifdef _MEGA_ENABLED_
			printlnFlashMsg(msg_grbl_timeout);
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.printFlashMsg(0,startRow,msg_grbl_timeout);
		#endif	
		break;
	case GRBL_RETURN_OVERFLOW:
		#ifdef _MEGA_ENABLED_
			printlnFlashMsg(msg_grbl_overflow);
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.printFlashMsg(0,startRow,msg_grbl_overflow);
		#endif	
		break;
	default: 
		#ifdef _MEGA_ENABLED_
			printlnFlashMsg(msg_grbl_unknown);
		#endif
		#ifdef _OLED_ENABLED_
			myOLED.printFlashMsg(0,startRow,msg_grbl_unknown);
		#endif	
		break;
	}
}

// Fuynction to display GRBL return is only used for MEGA board only
#ifdef _MEGA_ENABLED_

void printlnFlashMsg(const char* msgptr) {
	printFlashMsg(msgptr);
	Serial.println();
}

void printFlashMsg(const char* msgptr) {
	uint8_t i = myUtil::getFlashMsg(msgBuffer, msgptr, MSGBUFFER_SIZE);
	Serial.print(msgBuffer);
}

#endif


boolean sendFile() {

	#ifdef _MEGA_ENABLED_
		Serial.print("File to send: ");
		Serial.println(fileName);
	#endif

	#ifdef _OLED_ENABLED_
	  myOLED.clr(2, 7);
	  myOLED.print(0,2,">");
	  myOLED.print(fileName);
	#endif
/*
	// CHeck if file exists
	if (!sd.exists(fileName)) {
		#ifdef _OLED_ENABLED_
		  myOLED.printFlashMsg(0,3,msg_sd_not_found);
		#endif
		#ifdef _MEGA_ENABLED_
			printlnFlashMsg(msg_sd_not_found);
		#endif
		return false;
	}

*/
  myOLED.print(0,4,"Go");

  #ifdef _MEGA_ENABLED_
    Serial.print("Dump File: ");
    Serial.println(fileName);
  #endif
  #ifdef _OLED_ENABLED_
//	  myOLED.clr(2, 7);
//	  myOLED.print(0,2,">");
//	  myOLED.print(fileName);

	  myOLED.print(0,3,"Line:");
  #endif

  ifstream sdin(fileName);
  line_number = 0;

  while (sdin.getline(inBuffer, INBUFFER_SIZE, '\n') || sdin.gcount()) {
    int count = sdin.gcount();
    if (sdin.fail()) {
      #ifdef _MEGA_ENABLED_
        Serial.print("Partial long line");
      #endif
      sdin.clear(sdin.rdstate() & ~ios_base::failbit);
    } else if (sdin.eof()) {
      #ifdef _MEGA_ENABLED_
        Serial.print("Partial final line");  // sdin.fail() is false
      #endif
    } else {
      count--;  // Don’t include newline in count
      // cout << "Line " << ++line_number;
      // myUtil::f2s(sLine, ++line_number, 0, LINE_NUMBER_SIZE, 1,'0');
      ++line_number;
      #ifdef _MEGA_ENABLED_
        // Serial.print(sLine);
        Serial.print(line_number);
      #endif
      #ifdef _OLED_ENABLED_
			  // void printNum(uint8_t x, uint8_t line, long n, uint8_t base, uint8_t width, boolean fillZero);
      	myOLED.printNum(36, 3, line_number, 10, LINE_NUMBER_SIZE, true);
      #endif

    }

    #ifdef _MEGA_ENABLED_
      // cout << " [" << count << "]: " << buffer << endl;
      Serial.print(" [");
      Serial.print(count);
      Serial.print("]: ");
      Serial.print(inBuffer);
      Serial.println();
    #endif  

	  // (cnc.send("?",1,1));

	  #ifdef _OLED_ENABLED_
	    myOLED.clr(4,7);
	    myOLED.print(0,4,inBuffer);
	  #endif  
	  showGrblReturn(cnc.send(inBuffer,1,1), 4);

	  // ***** Remove the following for produciton  
    // if (line_number > 100) break;

  }
  #ifdef _MEGA_ENABLED_
  	printlnFlashMsg(msg_ui_complete);
  #endif

  #ifdef _OLED_ENABLED_
    myOLED.clr(3,7);
    myOLED.printFlashMsg(0,3,msg_ui_complete);
  #endif  

  return true;


}


#if defined(_OLED_ENABLED_) && defined(_KEY_ENABLED_)
// Allow user to select fileds in root directory

void readSdRoot() {
	#ifdef _MEGA_ENABLED_
		printlnFlashMsg(msg_ui_get_file_list);
	#endif
  for (uint16_t idx = 0; idx < 1024; idx++) {
    if (file.open(sd.vwd(), idx, O_READ)) {
      if (!file.isDir()) {
				#ifdef _MEGA_ENABLED_
		      if (file.getName(fileName, FILENAME_SIZE)) {
		      } else {
	        	myUtil::getFlashMsg(fileName, msg_sd_err_file_name, FILENAME_SIZE);
		      }
					Serial.print("> ");
					Serial.print(fileName);
					Serial.println(formatFileSize(file.fileSize()));
				#endif
        fileIdx[fileCnt++] = idx;
      }
      file.close();
      if (fileCnt >= FILEIDX_SIZE) break;
    }
  }
	#ifdef _MEGA_ENABLED_
		Serial.println();
	#endif
}

#ifdef _MEGA_ENABLED_

char * formatFileSize(long fileSize) {
	float _fileSize;
	if (fileSize < 1024) {
		myUtil::f2s(inBuffer, fileSize, 0, 4, _F2S_PAD_LEFT);
		memcpy(inBuffer+4," B",3);
	} else {
		_fileSize = (float) fileSize / 1024.0;
		if (_fileSize < 1024.0) {
			myUtil::f2s(inBuffer, _fileSize, 0, 4, _F2S_PAD_LEFT);
			memcpy(inBuffer+4," KB",4);
		} else {
			_fileSize /= 1024.0;
			myUtil::f2s(inBuffer, _fileSize, 0, 4, _F2S_PAD_LEFT);
			memcpy(inBuffer+4," MB",4);
		}
	}
	return inBuffer;
}

#endif

void showFileName() {
  myOLED.clr(3,7);
  myOLED.print(0,3,filePtr + 1,3);
  myOLED.print("/");
  myOLED.print(fileCnt,3);
  myOLED.print(0,4,fileName);
}

boolean selectFile() {
  int key;

  myOLED.printFlashMsg(0,2,msg_select_file);

  filePtr = 0;
  while (!fileSelected) {
    if (file.open(sd.vwd(), fileIdx[filePtr], O_READ)) {
      if (file.getName(fileName, FILENAME_SIZE)) {
      } else {
      	myUtil::getFlashMsg(fileName, msg_sd_err_file_name, FILENAME_SIZE);
      }
      showFileName();
      file.close();
    } else {
     	myUtil::getFlashMsg(fileName, msg_sd_err_file_open, FILENAME_SIZE);
    }  
    while (true) {
      key = myKey.getKey(true);
      if (key == KEY_SELECT) break;
      if (key == KEY_CANCEL) break;
      if ((key == KEY_UP) && (filePtr > 0)) break;
      if ((key == KEY_DOWN) && (filePtr < fileCnt - 1)) break;
    }

    if (key == KEY_SELECT) {
      fileSelected = true;
    } else if (key == KEY_CANCEL) {
    	return false;
    } else if (key == KEY_UP) {
      // double check for safety
      if (filePtr > 0)  filePtr--;
    } else if (key == KEY_DOWN) {
      // double check for safety
      if (filePtr < fileCnt - 1) filePtr++;
    }
  }

  return true;
}


#endif