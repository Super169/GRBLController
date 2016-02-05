//  myKeyPad.cpp by Super169 
//
//  Change Log:
//    v0.0.1 : 2015-11-19 : Initial Version
//

#include "myKeyPad.h"

myKeyPad::myKeyPad(uint8_t keyPin, uint8_t keyCnt, uint16_t* keys)
{
  _keyPin = keyPin;
  _keyCnt = keyCnt;
  _keys   = keys;
}

void myKeyPad::debugShowKeys() 
{
  #ifdef _MYKEYPAD_SERIAL_DEBUG_
  if (_keyCnt) {
    Serial.println("Key pad settings:");
    for (int idx = 0; idx < _keyCnt; idx++) {
      Serial.print("Key-");
      Serial.print(idx + 1, DEC);
      Serial.print(" : ");
      if (idx) {
        Serial.print(_keys[idx-1] + 1);
      } else {
        Serial.print("0");
      }
      Serial.print(" - ");
      Serial.print(_keys[idx]);
      Serial.println();
    }
  } else {
    Serial.println("No key defined");
  }
  #endif
}


int myKeyPad::getKeyPressed() {
  int val = analogRead(_keyPin);
  for (int k = 0; k < _keyCnt; k++) {
    if (val < _keys[k]) return k;
  }
  return -1;
}    

int myKeyPad::getKey(boolean waitForReleased) {
  // The key pad is not stable, it may generate wrong value in short time
  // Wait for key released may provide more accurate result, but it will have problem for continue press effect, so default not wait
  // For safety, check if same key returned for 1/100 seconds
  if (waitForReleased) {
    // wait 1/1000s to reduce the checking on analog pin.
    while (getKeyPressed() != -1) delay(1); 
  }

  int k0 = getKeyPressed();
  int k;
  delay(10);
  while ((k = getKeyPressed()) != k0) {
    k0 = k;
    delay(10);
  }
  return k0;
}

void myKeyPad::waitKey(uint8_t key, boolean waitForReleased) {
  if (waitForReleased) waitKeyReleased();
  while (getKeyPressed() != key) delay(1); 
}


uint8_t myKeyPad::waitKeys(uint8_t *keys, uint8_t keyCnt, boolean waitForReleased) {
  if (waitForReleased) waitKeyReleased();
  uint8_t key;
  boolean waitKey = true;
  uint8_t getKey = 0;
  // For safety, use other flag to control the loop instead of while (!getKey), as it may wait for key = 0
  while (waitKey) {
    key = getKeyPressed();
    for (int i = 0; i < keyCnt; i++) {
      if (key == keys[i]) {
        waitKey = false;
        getKey = keys[i];
      }    
    }
    delay(1);
  }  
  return getKey;
}

void myKeyPad::waitKeyReleased() {
  while (getKeyPressed() != -1) delay(1); 
}
