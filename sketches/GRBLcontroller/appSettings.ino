// Set the hardware for running GRBL Controller
#ifndef _APP_SETTIGNS_INC_
#define _APP_SETTIGNS_INC_

#define GRBL_SERIAL_BAUD     	115200
#define DEBUG_SERIAL_BAUD     115200

//
// PIN Connection:
//
//   Connection to Nano/Mini is the same as UNO
//
//   SD Card:
//               UNO     MEGA
//      CS    :  D10  :  D53    
//      SCK   :  D13  :  D52 
//      MOSI  :  D11  :  D51
//      MISO  :  D12  :  D50
//      VCC   :  5V   :  5V
//      GND   :  GND  :  GND
//    
//   OLED:
//               UNO     MEGA
//      VCC   :  5V   :  5V
//      GND   :  GND  :  GND
//      SCL   :  A5   :  D21
//      SDA   :  A4   :  D20
//
//   Key Pad
//               UNO     MEGA
//      VCC   :  5V   :  5V
//      GND   :  GND  :  GND
//      OUT   :  A0   :  A0
//
//      6-key pad
//       Left  Right Combo  Select Cancel Up    Down
//       170   345   408    510    684    844 - 932
//          200   380    480    650    800   900    960 
//
//
//   Arduino of CNC Shield
//               UNO     MEGA
//      TX    :  D0   :  D17
//      RX    :  D1   :  D16
//      GND   :  GND  :  GND
//      RST   :  D3   :  D3 
//

//  If no KeyPad, it will send the file auto.gcode when SEND_PIN is low
//
//     D2 : pullhigh, connect to GND for print


// Customization for different environment
//
// Only mega has dual serial port, so the serial debug only available for Mega
// It's assumed that Serial debug is always enabled for Mega.
//

#define GRBL_RESET_PIN 3

#include "myGRBL.h"
#ifdef _MEGA_ENABLED_
	myGRBL cnc(Serial2, GRBL_SERIAL_BAUD);
#else
	myGRBL cnc(Serial, GRBL_SERIAL_BAUD);
#endif

#include "msgSettings.ino"

#include "oledSettings.ino"

#include "keySettings.ino"

#if defined(_OLED_ENABLED_) && defined(_KEY_ENABLED_)
	#include "menuSettings.ino"
#endif



#endif