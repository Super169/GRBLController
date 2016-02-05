#ifndef _MENU_SETTINGS_INC_
#define _MENU_SETTINGS_INC_

#include <avr/pgmspace.h>
#include "myMenuItem.h"
#include "myMenu.h"


#define MENU_CONNECT_CNC	1

#define MENU_JOG_CONTINUE 21
#define MENU_JOG_SINGLE   22
#define MENU_CHECK_STATUS 23

#define MENU_SEND_FILE    3


#define MENU_SOFT_RESET   41
#define MENU_HARD_RESET   42

#define MENU_GO_HOME      431
#define MENU_SET_000      432
#define MENU_GO_000       433

#define MENU_PROBE_Z      441
#define MENU_SET_Z0       442


const char menu_1[] PROGMEM = "Connect CNC";
const char menu_2[] PROGMEM = "Jog Mode";
const char menu_3[] PROGMEM = "Send File";
const char menu_4[] PROGMEM = "Settings";

const char menu_21[] PROGMEM = "Continue";
const char menu_22[] PROGMEM = "Single Step";
const char menu_23[] PROGMEM = "Check Status";

const char menu_41[] PROGMEM = "Soft-Reset";
const char menu_42[] PROGMEM = "Hard-Reset";
const char menu_43[] PROGMEM = "Home Position";
const char menu_44[] PROGMEM = "Tune Z Position";


const char menu_431[] PROGMEM = "Machine Home";
const char menu_432[] PROGMEM = "Set 0,0,0";
const char menu_433[] PROGMEM = "Go 0,0,0";


const char menu_441[] PROGMEM = "Probe Z";
const char menu_442[] PROGMEM = "Set Z to 0";


myMenuItem subMenu_43[] = {myMenuItem(menu_431, MENU_GO_HOME), 
	                         myMenuItem(menu_432, MENU_SET_000),
	                         myMenuItem(menu_433, MENU_GO_000)
	                        };


myMenuItem subMenu_44[] = {myMenuItem(menu_441, MENU_PROBE_Z), 
	                         myMenuItem(menu_442, MENU_SET_Z0)};


myMenuItem subMenu_2[] = {myMenuItem(menu_21, MENU_JOG_CONTINUE), 
	                        myMenuItem(menu_22, MENU_JOG_SINGLE), 
	                        myMenuItem(menu_23, MENU_CHECK_STATUS)};

myMenuItem subMenu_4[] = {myMenuItem(menu_41, MENU_SOFT_RESET), 
	                        myMenuItem(menu_42, MENU_HARD_RESET), 
	                        myMenuItem(menu_43, subMenu_43, 3), 
	                        myMenuItem(menu_44, subMenu_44, 2)
	                      };

myMenuItem mainMenu[] = {myMenuItem(menu_1, MENU_CONNECT_CNC), 
	                       myMenuItem(menu_2, subMenu_2, 3), 
	                       myMenuItem(menu_3, MENU_SEND_FILE), 
	                       myMenuItem(menu_4, subMenu_4, 4)
	                      };

myMenu appMenu(myOLED, 3, 7, false,
               myKey, KEY_UP, KEY_DOWN, KEY_SELECT, KEY_CANCEL,
               mainMenu, 4, true, '>');


#endif