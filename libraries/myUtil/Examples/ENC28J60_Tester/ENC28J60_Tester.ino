#include <SPI.h>
#include "UIPEthernet.h"
#include <Wire.h>
#include <math.h>
#include "UTFT.h"

#include "SuperConn_ENC28J60.h"

EthernetClient client;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x28, 0x60};
char server[] = "api.yeelink.net";   // name address for yeelink API
uint16_t port = 80;

SuperConn_ENC28J60 myConn(client, mac, server, port);

void setup() {

	Serial.begin(57600);

	if (myConn.begin()==0) {
		Serial.println("Connection failed");
		while(1) ;
	}
	Serial.println("Connection success");


}

void loop() {
	
}