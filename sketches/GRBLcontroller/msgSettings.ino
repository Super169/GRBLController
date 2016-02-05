#ifndef _MSG_SETTIGNS_INC_
#define _MSG_SETTIGNS_INC_

#include "Arduino.h"


#if defined(_MEGA_ENABLED_) || defined(_OLED_ENABLED_)

  const char msg_grbl_go_connect[] PROGMEM = "Connecting CNC";

  const char msg_grbl_not_ready[] PROGMEM = "*> NOT REAFY";
  const char msg_grbl_ok[] PROGMEM = ">> OK";
  const char msg_grbl_no_connection[] PROGMEM = "*> NOT CONNECTED";
  const char msg_grbl_timeout[] PROGMEM = "*> TIMEOUT";
  const char msg_grbl_overflow[] PROGMEM = "*> OVERFLOW";
  const char msg_grbl_unknown[] PROGMEM = "*> UNKNOWN";

  const char msg_grbl_connected[] PROGMEM = "CNC Connected";
  const char msg_grbl_connect_fail[] PROGMEM = "Fail to connect CNC";

  const char msg_grbl_reset[] PROGMEM = "CNC Reset OK";

  const char msg_cnc_hard_reset[] PROGMEM = "Hard Reset CNC";

  const char msg_ui_complete[] PROGMEM = "Job Completed.";

	const char msg_sd_failed[] PROGMEM =  "SD: Failed";
	const char msg_sd_empty[] PROGMEM =  "SD: Empty";
	const char msg_sd_not_found[] PROGMEM = "SD: File Not Found";
	const char msg_sd_err_file_name[] PROGMEM = "SD: Error reading file name";
	const char msg_sd_err_file_open[] PROGMEM = "SD: Error in file.open";


	#ifdef _MEGA_ENABLED_

	  const char msg_grbl_erro_reset[] PROGMEM = "Error to reset GRBL";
	  const char msg_grbl_build_info[] PROGMEM = "Build Info: ";
	  const char msg_grbl_parser_state[] PROGMEM = "Parser state: ";
	  const char msg_grbl_current_state[] PROGMEM = "Current status: ";
	  
	  const char msg_ui_system_halt[] PROGMEM = "\nSystem Halt\n";
	  const char msg_ui_msg1[] PROGMEM = "Enter G-Code for testing";
		const char msg_ui_msg2[] PROGMEM = "(due to buffer size limitaiton, command with long return will become OVERFLOW)\n";

	  const char msg_ui_get_file_list[] PROGMEM = "Get file list from SD\n";
	#endif

#endif


#if defined(_OLED_ENABLED_) && defined(_KEY_ENABLED_)
	const char msg_select_file[] PROGMEM =  "[^][v] SELECT; [SEL] GO";
	const char msg_key_wait_select[] PROGMEM =  "[SELECT] to continue";
	const char msg_key_wait_select_cancel[] PROGMEM =  "[SEL/CAN] to contine";
	const char msg_key_get_select_cancel[] PROGMEM =  "[SELECT]  / [CANCEL]";
  const char msg_jog_single[] PROGMEM = "  - JOG Single";
  const char msg_jog_continue[] PROGMEM = "  - JOG Continue";

  const char msg_probe_Z[] PROGMEM = "Connect SCL & GND";
#endif

#if !defined(_KEY_ENABLED_)
  // modify this message if 
	const char msg_wait_gnd[] PROGMEM =  "D2 -> GND to Start";
#endif  


#endif // _MSG_SETTIGNS_INC_