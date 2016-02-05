#include "myMenu.h"

myMenu::myMenu(OLED12864& oled, uint8_t startRow, uint8_t endRow,  boolean lineMode,
               myKeyPad& keyPad, uint16_t keyUp, uint16_t keyDown, uint16_t keySelect, uint16_t keyCancel, 
               myMenuItem *mainMenu, uint8_t itemCnt, boolean showItemType, char subMenuInd) :
	_myOLED(oled), 
	_myKey(keyPad)

{
	config(startRow, endRow, lineMode, 
         keyUp, keyDown, keySelect, keyCancel,
         mainMenu, itemCnt, showItemType, subMenuInd);
/*
	_startRow = startRow;
	_endRow = endRow;
	_lineMode = lineMode;
	_maxRows = _endRow - _startRow + 1;
	_currDepth = 0;
	_menu[_currDepth].items = mainMenu;
	_menu[_currDepth].size = itemCnt;
	_menu[_currDepth].selection = -1;
	_screenOffset = 0;
	_currItem = 0;
	_keyUp = keyUp;
	_keyDown = keyDown;
	_keySelect = keySelect;
	_keyCancel = keyCancel;
	_showItemType = showItemType;
	_subMenuInd = subMenuInd;
*/	
}

myMenu::myMenu(OLED12864& oled, uint8_t startRow, uint8_t endRow,  boolean lineMode,
               myKeyPad& keyPad, uint16_t keyUp, uint16_t keyDown, uint16_t keySelect, uint16_t keyCancel, 
               myMenuItem *mainMenu, uint8_t itemCnt, boolean showItemType)  :
	_myOLED(oled), 
	_myKey(keyPad)
{
	config(startRow, endRow, lineMode, 
         keyUp, keyDown, keySelect, keyCancel,
         mainMenu, itemCnt, showItemType, '*');
}

myMenu::myMenu(OLED12864& oled, uint8_t startRow, uint8_t endRow,  boolean lineMode,
               myKeyPad& keyPad, uint16_t keyUp, uint16_t keyDown, uint16_t keySelect, uint16_t keyCancel, 
               myMenuItem *mainMenu, uint8_t itemCnt)  :
	_myOLED(oled), 
	_myKey(keyPad)
{
	config(startRow, endRow, lineMode, 
         keyUp, keyDown, keySelect, keyCancel,
         mainMenu, itemCnt, true, '*');
}


void myMenu::config(uint8_t startRow, uint8_t endRow,  boolean lineMode,
                    uint16_t keyUp, uint16_t keyDown, uint16_t keySelect, uint16_t keyCancel, 
                    myMenuItem *mainMenu, uint8_t itemCnt, boolean showItemType, char subMenuInd) {
	_startRow = startRow;
	_endRow = endRow;
	_lineMode = lineMode;
	_maxRows = _endRow - _startRow + 1;
	_currDepth = 0;
	_menu[_currDepth].items = mainMenu;
	_menu[_currDepth].size = itemCnt;
	_menu[_currDepth].selection = -1;
	_screenOffset = 0;
	_currItem = 0;
	_keyUp = keyUp;
	_keyDown = keyDown;
	_keySelect = keySelect;
	_keyCancel = keyCancel;
	_showItemType = showItemType;
	_subMenuInd = subMenuInd;
}


void myMenu::dummyTest() {
	_myOLED.print(0,1, "Just for fun");
}

void myMenu::clr() {
	_myOLED.clr(_startRow, _endRow);
}


void myMenu::showMenuItem(int idx, boolean inverse) {
	int displayRow = (int) _startRow + idx - _screenOffset;
	if ((displayRow < _startRow) || (displayRow > _endRow)) return;
  if (inverse) _myOLED.setInverse(true);
  if (_lineMode) _myOLED.clr(displayRow);
  if (_showItemType) {
  	if (_menu[_currDepth].items[idx].isSubMenu()) {
  		_myOLED.print(0, displayRow, _subMenuInd);
  	} else {
  		_myOLED.print(0, displayRow, ' ');
  	}
 		_myOLED.printFlashMsg(_menu[_currDepth].items[idx].getItemDesc());
  } else {
		_myOLED.printFlashMsg(0, displayRow, _menu[_currDepth].items[idx].getItemDesc());
  }
  if (inverse) _myOLED.setInverse(false);
}

void myMenu::showMenu() {
	// Maximum 4 options, no scrolling at this version
	clr();
	uint8_t numRows = _menu[_currDepth].size - _screenOffset;
	if (numRows > _maxRows) numRows = _maxRows;

	for (int idx = 0; idx < numRows; idx++) {
  	showMenuItem(_screenOffset + idx, false);
  }
  showMenuItem(_currItem, true);
}

void myMenu::changeMenuIdx(int newItem) {
  if (_currItem == newItem) return;
  showMenuItem(_currItem, false);
  _currItem = newItem;
  showMenuItem(_currItem, true);
}


uint16_t myMenu::getMenuSelection() {
	boolean itemSelected = false;
	uint16_t selectedItem = 0;
	myMenuItem *currMenu;

	showMenu();
	while (!itemSelected) {
		int key = _myKey.getKey(true);

		// cannot use switch / case as it is not checking against constant
		if (key == _keyUp) {
			if (_currItem > 0) {
				if (_currItem == _screenOffset) {
					_screenOffset--;
					showMenu();
				}
				changeMenuIdx(_currItem - 1);
			}
		} else if (key == _keyDown) {
			if (_currItem < _menu[_currDepth].size - 1) {
				if (_startRow + _currItem - _screenOffset >= _endRow) {
					_screenOffset++;
					showMenu();
				}
				changeMenuIdx(_currItem + 1);
			}
		} else if (key == _keySelect) {
			if (_menu[_currDepth].items[_currItem].isSubMenu())  {
				currMenu = _menu[_currDepth].items;
				_menu[_currDepth].selection = _currItem;
				_menu[_currDepth].screenOffset = _screenOffset;
				_currDepth++;
				_menu[_currDepth].items = currMenu[_currItem].getSubMenu();
				_menu[_currDepth].size = currMenu[_currItem].getSubItemCnt();
				_screenOffset = 0;
	    	_currItem = 0;
	    	showMenu();
			} else {
				itemSelected = true;
				selectedItem = _menu[_currDepth].items[_currItem].getReturnCode();
			}
		} else if (key == _keyCancel) {
    	if (_currDepth > 0) {
    		_currDepth--;
    		_currItem = _menu[_currDepth].selection;
    		_screenOffset = _menu[_currDepth].screenOffset;
    		showMenu();
    	}
		}

	}
	return selectedItem;
}