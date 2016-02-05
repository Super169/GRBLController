#include "myGRBL.h"

#define SERIAL_BAUD     115200

// Buffer for user input.
#define BUFFER_SIZE 128
char inBuffer[BUFFER_SIZE+1];


myGRBL cnc(Serial2, SERIAL_BAUD);

void setup() {
	Serial.begin(SERIAL_BAUD);

	// CNC.begin(SERIAL_BAUD);
	cnc.begin();

	Serial.println("CNC Mega Testing");
	
	int r;
	if ((r = cnc.softReset()) < GRBL_RETURN_ERROR) {
		Serial.println(cnc.getBuffer());
	} else {
		Serial.println("Error to reset GRBL")	;
		showGrblReturn(r);
		Serial.print("\nSystem Halt")	;
		while (1);
	};

	if (cnc.send("$I", 1) < GRBL_RETURN_ERROR) {
		Serial.print("Build Info: ");
		Serial.print(cnc.getBuffer());
	}

	if (cnc.send("$G", 1) < GRBL_RETURN_ERROR) {
		Serial.print("Parser state: ");
		Serial.print(cnc.getBuffer());
	}

	if (cnc.send("?", 1) < GRBL_RETURN_ERROR) {
		Serial.print("Current status: ");
		Serial.println(cnc.getBuffer());
	}

	Serial.println("Enter G-Code for testing (due to buffer size limitaiton, command with long return will become OVERFLOW)\n");

}

void loop() {
	int r;
	int ptr = 0;
	if (Serial.available()) {
		while (Serial.available()) {
			inBuffer[ptr++] = (char) Serial.read();
			delay(1);
			if (ptr >= BUFFER_SIZE) {
				// It shoudld not happened
				// Clear buffer and return and error
				inBuffer[ptr] = 0;
				Serial.println("Input too long:");
				Serial.println(inBuffer);
				ptr = 0;
				// clearCncBuffer();
				cnc.clearConnBuffer();
				break;
			}
		}

		if (ptr) {
			inBuffer[ptr] = 0;
			showGrblReturn(r = cnc.send(inBuffer));  // In case _SERIAL_DEBUG_ is disabled
			// any checking on return r

		}
	}
}

// Just for testing when _SERIAL_DEBUG_ of GRBL is disabled
void showGrblReturn(int err) {
	switch (err) {
	case GRBL_RETURN_NOTREADY:
		Serial.println("*> NOT READY");
		break;
	case GRBL_RETURN_OK:
		Serial.println(">> ok");
		break;
	case GRBL_RETURN_END_OK:
		Serial.print(">> ");
		Serial.print(cnc.getBuffer());
		break;
	case GRBL_RETURN_OTHERS:
		Serial.print("O> ");
		Serial.print(cnc.getBuffer());
		break;
	case GRBL_RETURN_ERROR:
		Serial.print("E> ");
		Serial.print(cnc.getBuffer());
		break;
	case GRBL_RETURN_TIMEOUT:
		Serial.println("*> TIMEOUT!!");
		break;
	case GRBL_RETURN_OVERFLOW:
		Serial.println("*> OVERFLOW!!");
		break;
	default: 
		Serial.println("?> UNKNOWN!!");
		break;
	}
}
