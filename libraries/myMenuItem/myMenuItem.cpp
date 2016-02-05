#include "myMenuItem.h"

myMenuItem::myMenuItem(const char *itemDesc, uint16_t returnCode) {
	_itemDesc = itemDesc;
	_returnCode = returnCode;
	_subItemCnt = 0;
}

myMenuItem::myMenuItem(const char *itemDesc, myMenuItem subMenuItem[], uint8_t subItemCnt) {
	_itemDesc = itemDesc;
	_returnCode = 0;
	_subItemCnt = subItemCnt;
	_subMenuItem = subMenuItem;
}



const char* myMenuItem::getItemDesc() {
	return _itemDesc;
}


uint16_t myMenuItem::getReturnCode() {
	return _returnCode;
}

boolean myMenuItem::isSubMenu() {
	return (_subItemCnt > 0);
}

myMenuItem* myMenuItem::getSubMenu() {
	return _subMenuItem;
}

uint8_t myMenuItem::getSubItemCnt() {
	return _subItemCnt;
}
