#ifndef _MENU_SETTINGS_INC_
#define _MENU_SETTINGS_INC_

#include <avr/pgmspace.h>
#include "myMenuItem.h"

const char menu_01[] PROGMEM = "Connect CNC";
const char menu_02[] PROGMEM = "Jog Mode";
const char menu_03[] PROGMEM = "Send File";
const char menu_04[] PROGMEM = "BT Connect";
const char menu_05[] PROGMEM = "Settings";

const char menu_0201[] PROGMEM = "Jog X";
const char menu_0202[] PROGMEM = "Jog Y";
const char menu_0203[] PROGMEM = "Jog Z";

const char menu_020101[] PROGMEM = "Up";
const char menu_020102[] PROGMEM = "Down";


const char menu_0301[] PROGMEM = "Apple";
const char menu_0302[] PROGMEM = "Orange";
const char menu_0303[] PROGMEM = "Banana";
const char menu_0304[] PROGMEM = "Berry";

myMenuItem subMenu_0201[] = {myMenuItem(menu_020101, 20101), myMenuItem(menu_020102, 20102)};

myMenuItem subMenu_02[] = {myMenuItem(menu_0201, subMenu_0201, 2), 
	                         myMenuItem(menu_0202, 202), 
	                         myMenuItem(menu_0203, 203)};
myMenuItem subMenu_03[] = {myMenuItem(menu_0301, 301), myMenuItem(menu_0302, 302), myMenuItem(menu_0303, 303), myMenuItem(menu_0304, 304)};

myMenuItem mainMenu[] = {myMenuItem(menu_01,1), 
	                       myMenuItem(menu_02, subMenu_02, 3), 
	                       myMenuItem(menu_03, subMenu_03, 4), 
	                       myMenuItem(menu_04, 4),
	                       myMenuItem(menu_05, 5),
	                      };

#endif