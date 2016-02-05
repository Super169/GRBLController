#include "myUtil.h"

unsigned int myUtil::availableMemory(unsigned int max)	 {
  unsigned int size = max; 
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL) ;

  free(buf);

  return size;
}


void myUtil::myDelay(unsigned long idle_period) {
	unsigned long start_time = millis();
	while (millis() - start_time < idle_period);
}

uint8_t myUtil::f2s(char* buffer, double data, uint8_t decimal, uint8_t width, uint8_t padMode, char padChar) {

  if (width == 0) return false;

  double absData, adjData;
  // for safety, don't use unsigned int here, as the length (e.g. len_rem) can be negative after calculation
  int len_ne, len_int, len_dec, len_rem, data_len;

  if (data < 0) {
    len_ne = 1;
    absData = - data;
  } else {
    len_ne = 0;
    absData = data;
  }
  len_int = getLen(absData);
  if (len_ne + len_int > width) {
    return fillError(buffer, width);
  }

  len_dec = (decimal > 0 ? 1 : 0);
  if (decimal) {
    len_rem = width - (len_ne + len_int + len_dec);
    if (len_rem > decimal) {
      len_rem = decimal;
    } else {
      if (len_rem <= 0) {
        len_rem = 0;
        len_dec = 0;  // no need to display decimal place
      }
    }
  } else {
    len_rem = 0;
  }

  data_len = len_ne + len_int + len_dec + len_rem;
  adjData = absData + 0.5 / getFactor(len_rem);

  // check if addition digits after round up (e.g. 999.5 -> 1000.0)
  if (getLen(adjData) > len_int) {
    len_int++;
    if (len_ne + len_int > width) {
      return fillError(buffer, width);
    }
    if ((width > 0) && (data_len >= width)) {
      // try to reduce decimal to fit the data
      if (len_rem > 0) {
        len_rem--;
        adjData = absData + 0.5 / getFactor(len_rem);
        if (len_rem == 0) len_dec = 0;
      }
    }
    data_len = len_ne + len_int + len_dec + len_rem;
  }
  
  // Fill buffer with 0 as terminator anywhere, * size = widht + 1
  for (int idx = 0; idx <= width; idx++) buffer[idx] = 0;

  if (len_rem == 0) len_dec = 0;
  data_len = len_ne + len_int + len_dec + len_rem;

  uint8_t ptr = 0;
  // pad left
  if (padMode == _F2S_PAD_LEFT) while (ptr < width - data_len) buffer[ptr++] = padChar;

  if (len_ne) buffer[ptr++] = '-';

  unsigned long adjInt = (unsigned long) adjData;

  setNum(buffer, ptr, adjInt, len_int);
  ptr += len_int;
  if (len_rem > 0) {
    buffer[ptr++] = '.';
    unsigned long adjRem = getFactor(len_rem) * (adjData - adjInt);
    setNum(buffer, ptr, adjRem, len_rem);
    ptr += len_rem;
  }
  // pad right
  if (padMode == _F2S_PAD_RIGHT) while (ptr < width) buffer[ptr++] = padChar;

  // Just for safety, should already filled with 0 before
  buffer[ptr] = 0;
  return ptr;
  
}


double myUtil::getFactor(uint8_t len) {
  unsigned long ul_decimal = 1;
  for (int i = 0; i < len; i++) ul_decimal *= 10;
  return ((double) ul_decimal);
}

uint8_t myUtil::fillError(char* buffer, uint8_t width) {
  // * it's expected that the buffer has size = width+1
  for (int idx=0; idx < width; idx++) buffer[idx] = '-';
  buffer[width] = 0;
  return 0;
}

uint8_t myUtil::setNum(char* buffer, uint8_t idx, unsigned long data, uint8_t len) {

  idx += len- 1;
  for (int i = 0; i < len; i++) {
    buffer[idx - i] = '0' + (data % 10);
    data /= 10;
  }
}

uint8_t myUtil::getLen(unsigned long data) {
  // Use simple comparison for quick checking of data length
  // maximum value = 4294967040; i.e. max 10 digits
  if (data < 0) data = -data;
  if (data >= 1000000000) return 10;
  if (data >= 100000000) return 9;
  if (data >= 10000000) return 8;
  if (data >= 1000000) return 7;
  if (data >= 100000) return 6;
  if (data >= 10000) return 5;
  if (data >= 1000) return 4;
  if (data >= 100) return 3;
  if (data >= 10) return 2;
  return 1;
}


double myUtil::adjAverage(double* samples, uint8_t sample_cnt) {

  if (sample_cnt == 0) return 0;

  if (sample_cnt == 1) {
    return samples[0];
  }

  double sum, max, min, avg;
  sum = max = min = samples[0];
  for (int i = 1; i < sample_cnt; i++) {
    if (samples[i] > max) max = samples[i];
    if (samples[i] < min) min = samples[i];
    sum += samples[i];
  }
/*
  Serial.print("sum: ");
  Serial.print(sum);
  Serial.println();
*/
  if (sample_cnt < 5) {
    // Just calculate the average of samples
    avg = sum / sample_cnt;
  } else {
    avg = (sum - max - min) / (sample_cnt - 2);
  }
  return avg;
}

uint8_t myUtil::getFlashMsg(char * localMsgPtr, const char * flashMsgPtr, uint8_t buffer_size) {
	uint8_t idx = 0;
	while ((idx < buffer_size - 1) && (localMsgPtr[idx] =  pgm_read_byte(&flashMsgPtr[idx]))) {idx++;}
	localMsgPtr[idx] = 0;
  return idx;
}