#define __PROG_TYPES_COMPAT__
#include <avr/pgmspace.h>

const char appName[] PROGMEM = "GRBL Controller v0.1";

// About changes of PROGMEM
//   1.6.0: https://github.com/arduino/Arduino/wiki/1.6-Frequently-Asked-Questions
//   1.6.5: http://forum.arduino.cc/index.php?topic=339011.0

#include <Wire.h>
#include "OLED12864.h"

boolean enableBuffer = true;
boolean directDraw = true;

//  OLED_1306i2c - 0.96" OLED
//  OLED_1106i2c - 1.3" OLED
OLED12864 myOLED(OLED_1306i2c, enableBuffer, directDraw);

#include "myKeyPad.h"
// modify this setting for your keypad if needed
#define KEY_PIN    0
#define KEY_COUNT  7
uint16_t adc_key[KEY_COUNT] = {200, 380, 480, 650, 800, 900, 1000};

myKeyPad myKey(KEY_PIN, KEY_COUNT, adc_key);

#define KEY_LEFT   0
#define KEY_RIGHT  1
#define KEY_UP     5
#define KEY_DOWN   6
#define KEY_SELECT 3
#define KEY_CANCEL 4
#define KEY_COMBO  2

#include "myUtil.h"
#include "myMenuItem.h"

const char menu_01[] PROGMEM = "CNC Connect";
const char menu_02[] PROGMEM = "Jog Mode";
const char menu_03[] PROGMEM = "Send File";
const char menu_04[] PROGMEM = "BT Connect";

/*
const char menu_0201[] PROGMEM = "Jog X";
const char menu_0202[] PROGMEM = "Jog Y";
const char menu_0203[] PROGMEM = "Jog Z";

*/

myMenuItem mi_01(menu_01, 1);
myMenuItem mi_02(menu_02, 2);
myMenuItem mi_03(menu_03, 3);
myMenuItem mi_04(menu_04, 4);


myMenuItem mainMenu[] = {myMenuItem(menu_01,1), myMenuItem(menu_02, 2), myMenuItem(menu_03, 3), myMenuItem(menu_04, 4)};


#define MENU_ITEM_COUNT 4


#define MENU_OFFSET 3

uint8_t currItem;

char buffer[80];

void setup() {
  Serial.begin(57600);
  myUtil::getFlashMsg(buffer, appName, 80);
  Serial.println(buffer);

  myOLED.begin();
  myOLED.clr();
  myOLED.printFlashMsg(0,0, (char *) appName);

/*
  Serial.println("My Tester - Menu");
  myUtil::getFlashMsg(buffer, menu_01, 80);
  Serial.println(buffer);
  myUtil::getFlashMsg(buffer, mi_02.getItemDesc(), 80);
  Serial.println(buffer);
  myUtil::getFlashMsg(buffer, mainMenu[3].getItemDesc(), 80);
  Serial.println(buffer);
*/
  showMenu(); 
}


void loop()
{
  int newItem;
  int key = myKey.getKey(true);
  switch (key) {
    case KEY_UP:
      newItem = currItem - 1;
      if (newItem >= 0) changeMenuIdx(newItem);
      break;
    case KEY_DOWN:  
      newItem = currItem + 1;
      if (newItem < MENU_ITEM_COUNT) changeMenuIdx(newItem);
      while (myKey.getKeyPressed() != -1);
      break;
  }
//  while (myKey.getKey() != -1);
}

void changeMenuIdx(int newItem) {
  if (currItem == newItem) return;
  showMenuItem(currItem, false);
  currItem = newItem;
  showMenuItem(currItem, true);
}


void showMenu() {
  for (int idx = 0; idx < 4; idx++) {
    showMenuItem(idx, false);
  }
  showMenuItem(currItem, true);
}

void showMenuItem(int idx, boolean inverse) {
  if (inverse) myOLED.setInverse(true);
  myOLED.printFlashMsg(0, MENU_OFFSET + idx, mainMenu[idx].getItemDesc());
  if (inverse) myOLED.setInverse(false);
}

