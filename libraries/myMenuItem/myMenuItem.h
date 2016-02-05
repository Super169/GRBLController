#ifndef _myMENUITEM_H_
#define _myMENUITEM_H_

#define _MYMENUITEM_SERIAL_DEBUG_
#include "Arduino.h"

class myMenuItem {

  public:
    myMenuItem(const char *itemDesc, uint16_t returnCode);
    myMenuItem(const char *itemDesc, myMenuItem subMenuItem[], uint8_t subItemCnt);

    const char* getItemDesc();
    uint16_t getReturnCode();
    boolean isSubMenu();
    myMenuItem* getSubMenu();
    uint8_t getSubItemCnt();

	private:

	private:
		const char    *_itemDesc;	
		uint16_t	  _returnCode;
		uint8_t       _subItemCnt;
		myMenuItem    *_subMenuItem;
};
#endif