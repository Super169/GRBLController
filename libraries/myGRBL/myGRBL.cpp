//  myGRBL.cpp by Super169 
//
//  Change Log:
//    v0.0.1 : 2015-11-09 : Initial Version
//

#include "myGRBL.h"

myGRBL::myGRBL(HardwareSerial& hwSerial, long speed) : 
	_conn(hwSerial)
{
	_speed = speed;
	_ready = false;
}

uint8_t myGRBL::begin() {
	if (_connected) _conn.end();
	_conn.begin(_speed);
	_connected = true;
	uint8_t retCode = softReset();
	if (!_ready) {
		_conn.end();
		_connected = false;
	}
	return retCode;
}

uint8_t myGRBL::softReset() {
	unsigned long curr_timeout;
	uint8_t retCode;
	if (!_connected) return GRBL_RETURN_NO_CONNECTION;
	clearBuffer();
	_conn.write(0x18);
	// For safety, wait 1 seconds for GRBL Reset
	delay(2000);

	// Set ready to true to enable checking, enabled timeout in checking
	curr_timeout = _grbl_timeout;
	_grbl_timeout = GRBL_RESET_TIMEOUT;
	_ready = true;

  retCode = waitForGrbl();

  _ready = (retCode == GRBL_RETURN_GRBL);
  _grbl_timeout = curr_timeout;

	return  retCode;
}

void myGRBL::clearConnBuffer() {
	while (_conn.available()) {
		_conn.read();
		delay(1);
	}
}

char * myGRBL::getBuffer() {
	return _buffer;
}

void myGRBL::clearBuffer() {
	memset(_buffer, 0, BUFFER_SIZE + 1);
	_ptr = 0;
}

uint8_t myGRBL::waitForGrbl() {

	if (!_ready) return GRBL_RETURN_NOTREADY;

	#ifdef _MYGRBL_SERIAL_DEBUG_
		Serial.print("::  myGRBL::waitForGrbl - start ");
	#endif	

	unsigned long t_start = millis();

	// wait for return
	while ((!_conn.available()) && ((_grbl_timeout == 0) || ((millis() - t_start) < _grbl_timeout)) );
	// while (!_conn.available());

	// Check for timeout
	if (!_conn.available()) { 
	#ifdef _MYGRBL_SERIAL_DEBUG_
		Serial.println(" - timeout");
	#endif	
		return  GRBL_RETURN_TIMEOUT;
	}

	#ifdef _MYGRBL_SERIAL_DEBUG_
		Serial.print(" - return");
	#endif	


	clearBuffer();

	while (_conn.available()) {
		if (_ptr < BUFFER_SIZE) {
			_buffer[_ptr++] = (char) _conn.read();
			delay(1);
		} else {
			// Flush buffer and return as OverFlow
			clearConnBuffer();
			#ifdef _MYGRBL_SERIAL_DEBUG_
				Serial.println(" - {Overflow}");
			#endif	
			return GRBL_RETURN_OVERFLOW;
		}
	}	
	_buffer[_ptr] = 0;

	#ifdef _MYGRBL_SERIAL_DEBUG_
		Serial.print(" >> ");
		Serial.println(_buffer);
	#endif	

	if (_ptr > 3) {
		// Special return after reset
		if (_ptr > 6) {
			if ((_buffer[0] == 0x0D ) && (_buffer[1] == 0x0A) && (_buffer[2] == 'G' ) &&
			    (_buffer[3] == 'r' ) && (_buffer[4] == 'b') && (_buffer[5] == 'l' ))  {
				return GRBL_RETURN_GRBL;
			}
		}

		// check End with OK
		if ((_buffer[_ptr-4] == 'o' ) && (_buffer[_ptr-3] == 'k') &&
		    (_buffer[_ptr-2] == 0x0D ) && (_buffer[_ptr-1] == 0x0A))  {
			if (_ptr == 4)	 return GRBL_RETURN_OK;
			return GRBL_RETURN_END_OK;
		}
		if (_ptr > 6) {
			if ((_buffer[0] == 'e' ) && (_buffer[1] == 'r') && (_buffer[2] == 'r' ) &&
			    (_buffer[3] == 'o' ) && (_buffer[4] == 'r') && (_buffer[5] == ':' ))  {
				return GRBL_RETURN_ERROR;
			}
		}
	}

	return GRBL_RETURN_OTHERS;
}

uint8_t myGRBL::send(char *cmd, uint8_t addx0D, uint8_t waitGrbl) {

	#ifdef _MYGRBL_SERIAL_DEBUG_
		Serial.print("::  myGRBL::send >> ");
		Serial.println(cmd);
	#endif	

	if (!_ready) return GRBL_RETURN_NOTREADY;
	// Clear GRBL input buffer
	clearConnBuffer();

	_conn.print(cmd);
	if (addx0D) _conn.write(0x0D);
	if (!waitGrbl) return 0;
	return showGrblReturn(waitForGrbl());
}

void myGRBL::print(char *cmd) {
	_conn.print(cmd);
}

uint8_t myGRBL::showGrblReturn(uint8_t r) {
	#ifdef _MYGRBL_SERIAL_DEBUG_

		switch (r) {
		case GRBL_RETURN_NOTREADY:
			Serial.print(":: GRBL Return NOT READY");
			break;
		case GRBL_RETURN_OK:
			Serial.print(":: GRBL Return ok");
			break;
		case GRBL_RETURN_END_OK:
			Serial.println(":: GRBL Return end with ok:");
			Serial.print(_buffer);
			break;
		case GRBL_RETURN_OTHERS:
			Serial.println(":: GRBL Return others:");
			Serial.print(_buffer);
			break;
		case GRBL_RETURN_ERROR:
			Serial.println(":: GRBL Return ERROR:");
			Serial.print(_buffer);
			break;
		case GRBL_RETURN_TIMEOUT:
			Serial.println(":: GRBL Return TIMEOUT!!");
			break;
		case GRBL_RETURN_OVERFLOW:
			Serial.println(":: GRBL Return OVERFLOW!!");
			break;
		default: 
			Serial.println(":: GRBL Return UNKNOWN!!");
			break;
		}

	#endif
	return r;
}