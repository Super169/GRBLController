#ifndef _myMENU_H_
#define _myMENU_H_

#define _MYMENU_SERIAL_DEBUG_
#include "Arduino.h"
#include <Wire.h>
#include "OLED12864.h"
#include "myKeyPad.h"
#include "myMenuItem.h"

#define MAX_MENU_DEPTH  5

typedef struct MENU_STRUCTURE {
    myMenuItem *items;
    uint8_t size;
    uint8_t screenOffset;
    int selection;
}  menu_t;

class myMenu {

  public:
    myMenu(OLED12864& oled, uint8_t startRow, uint8_t endRow, boolean lineMode,
           myKeyPad& keyPad, uint16_t keyUp, uint16_t keyDown, uint16_t keySelect, uint16_t keyCancel, 
           myMenuItem *mainMenu, uint8_t itemCnt, boolean showItemType, char subMenuInd);

    myMenu(OLED12864& oled, uint8_t startRow, uint8_t endRow, boolean lineMode,
           myKeyPad& keyPad, uint16_t keyUp, uint16_t keyDown, uint16_t keySelect, uint16_t keyCancel, 
           myMenuItem *mainMenu, uint8_t itemCnt, boolean showItemType);

    myMenu(OLED12864& oled, uint8_t startRow, uint8_t endRow, boolean lineMode,
           myKeyPad& keyPad, uint16_t keyUp, uint16_t keyDown, uint16_t keySelect, uint16_t keyCancel, 
           myMenuItem *mainMenu, uint8_t itemCnt);

    void clr();
    void showMenu();
    uint16_t getMenuSelection();

    void dummyTest();

  private:

    void config(uint8_t startRow, uint8_t endRow,  boolean lineMode,
                uint16_t keyUp, uint16_t keyDown, uint16_t keySelect, uint16_t keyCancel, 
                myMenuItem *mainMenu, uint8_t itemCnt, boolean showItemType, char subMenuInd);
    void showMenuItem(int idx, boolean inverse);
    void changeMenuIdx(int newItem) ;

  private:
    OLED12864& _myOLED;
    boolean    _lineMode;
    myKeyPad&  _myKey;  
    menu_t _menu[MAX_MENU_DEPTH];
    uint8_t _startRow, _endRow, _maxRows;
    uint8_t _currDepth = 0;
    uint8_t _screenOffset;
    uint8_t _currItem = 0;
    uint16_t _keyUp;
    uint16_t _keyDown;
    uint16_t _keySelect;
    uint16_t _keyCancel;
    boolean  _showItemType;
    char     _subMenuInd;
};

#endif