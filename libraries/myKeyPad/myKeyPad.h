/*
  20151103 - Add optional parameter padChar for f2s

*/
#ifndef _myKEYPAD_H_
#define _myKEYPAD_H_

#define _MYKEYPAD_SERIAL_DEBUG_

#include "Arduino.h"


class myKeyPad {

    public:
        myKeyPad(uint8_t, uint8_t, uint16_t* );
        int getKey(boolean waitForReleased);
        int getKey() {return getKey(false); }
        void debugShowKeys();
        int getKeyPressed();
        void waitKeyReleased();
        void waitKey(uint8_t key, boolean waitForReleased);
        void waitKey(uint8_t key) { return waitKey(key, true); }
        uint8_t waitKeys(uint8_t *keys, uint8_t keyCnt, boolean waitForReleased);
        uint8_t waitKeys(uint8_t *keys, uint8_t keyCnt) { return waitKeys(keys, keyCnt, true); }

	private:

	private:
        uint8_t         _keyPin;
        uint8_t         _keyCnt;
        uint16_t        *_keys;
};

#endif