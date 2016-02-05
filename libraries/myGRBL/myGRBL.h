/*
  20151103 - Add optional parameter padChar for f2s

*/
#ifndef _myGRBL_H_
#define _myGRBL_H_

// #define _MYGRBL_SERIAL_DEBUG_

#include "Arduino.h"
#include "HardwareSerial.h"

// Would be better to incrase in buffer size of HWSerial for GRBL
// As 64 bytes cannot handle some return from GRBL
//    <Arduino>\hardware\arduino\avr\cores\arduino\HardwareSerial.h
//       #define SERIAL_RX_BUFFER_SIZE 64   (change this value to 128 if possible)  
#define BUFFER_SIZE		128
#define GRBL_DEFAULT_TIMEOUT 5000  
#define GRBL_RESET_TIMEOUT   5000  

#define GRBL_RETURN_NO_CONNECTION 100    
#define GRBL_RETURN_NOTREADY 99
#define GRBL_RETURN_TIMEOUT  98
#define GRBL_RETURN_OVERFLOW 90
#define GRBL_RETURN_ERROR    10
#define GRBL_RETURN_OK       0
#define GRBL_RETURN_END_OK   1
#define GRBL_RETURN_GRBL     2
#define GRBL_RETURN_OTHERS   9

class myGRBL {

	public:

	  myGRBL(HardwareSerial& hwSerial, long speed);
  	uint8_t begin();
  	void setTimeout(unsigned long ms) { _grbl_timeout = ms; }
	  uint8_t softReset();
	  boolean isConnected() { return _connected; }
		boolean isReady() { return _ready; }
	  uint8_t send(char *cmd, uint8_t addx0D, uint8_t waitGrbl); 
	  uint8_t send(char *cmd, uint8_t addx0D) {return send(cmd, addx0D, 1);}
	  uint8_t send(char *cmd) {return send(cmd, 0, 1);}
	  char * getBuffer();
	  void print(char *cmd);
	  uint8_t showGrblReturn(uint8_t r);
		void clearConnBuffer();

	private:
		void clearBuffer();
		uint8_t waitForGrbl();

	private:
		HardwareSerial& _conn;
    long   					_speed;
    boolean         _connected;
    boolean         _ready;
    char            _buffer[BUFFER_SIZE + 1];  // for safety, add one for zero terminator, it should always be ZERO
    uint8_t					_ptr;
    unsigned long	  _grbl_timeout = GRBL_DEFAULT_TIMEOUT;
};

#endif