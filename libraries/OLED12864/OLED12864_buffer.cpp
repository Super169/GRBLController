
#include <Wire.h>
#include <avr/pgmspace.h>
#include <OLED12864.h>

uint16_t OLED12864::fillBuffer(uint8_t c, uint16_t startPos, uint16_t endPos)
{

  if (!_enableBuffer) return 0;
  if (startPos > endPos) return 0;

  #ifdef _OLED12864_DEBUG_
    Serial.print("filleBuffer : ");
    Serial.print(startPos);
    Serial.print(" : ");
    Serial.print(endPos);
    Serial.println();
  #endif

  for (uint16_t i = startPos; i <= endPos; i++) _buffer[i] = c;
  return (endPos - startPos + 1);
}

void OLED12864::scrollLine(uint8_t direction, uint8_t lines)
{
  if (!_enableBuffer) return;

  // This is not a live scrolling, just scroll the data immediately
  // Buffer scrolling only, it will have no effect if buffer is not enabled
  // Fix:  MUST show after scroll, otherwise it may have problem as other print will draw on screen immedicately
  uint16_t offset;
  switch (direction)
  {
    case OLED_SCROLL_UP:
      // Only handle line scroll at this moment
      offset = lines * OLED_WIDTH;
      for (int i = 0; i < BUFFER_SIZE - offset; i++) _buffer[i] = _buffer[i + offset];
      for (int i = BUFFER_SIZE - offset; i < BUFFER_SIZE; i++) _buffer[i] = 0x00;
        show();
      break;
    default:
      break;
  }
}

uint8_t OLED12864::getBuffer(uint16_t pos)
{
  if (!_enableBuffer) return 0;

  return _buffer[pos];
}

// No counting on bit pattern, return expected bits if no error
uint8_t OLED12864::drawBuffer(uint16_t bPos, uint8_t bits, uint8_t mode, uint8_t expBitCount)
{
  if (!_enableBuffer) return 0;

  if ((bPos < 0) || (bPos >= BUFFER_SIZE)) return 0;

  switch (mode) 
  {
    case 0:  // OR (Add)
      _buffer[bPos] |= bits;
      break;
    case 1:  // AND NOT (Clear)
      _buffer[bPos] &= ~(bits);
      break;
    case 2:  // XOR
      _buffer[bPos] ^= bits;
      break;
    default:
      expBitCount = 0;
  }
  if (_directDraw) showBuffer(bPos, 1);
  return expBitCount;
}

uint16_t OLED12864::drawBits(uint16_t bPos, uint8_t startBit, uint8_t endBit, uint8_t mode)
{

  if (!_enableBuffer) return 0;
  
  uint8_t drawByte = 0;
  if (startBit > endBit) swap(startBit, endBit);
  
  if  ((startBit == 0) && (endBit == 7))  // fill all bits
    drawByte = 0xFF;
  else 
    for (uint8_t i = startBit; i <= endBit; i++) drawByte |= _BV(i);
  
  uint16_t rtn = endBit - startBit + 1;
  
  switch (mode)
  {
    case 0:
      _buffer[bPos] |= drawByte;
      break;
    case 1:
      _buffer[bPos] &= ~drawByte;
      break;
    case 2:  
      _buffer[bPos] ^= drawByte;
      break;
    default:
      rtn = 0;
  }

  return rtn;
}

uint16_t OLED12864::drawVLine(uint8_t x, uint8_t y1, uint8_t y2, uint8_t mode)
{

  if (!_enableBuffer) return 0;

  int rtn = 0;
  
  if (y1 == y2) return drawPixel(x, y1, mode);
  
  if (y1 > y2) swap(y1, y2);
  
  uint16_t startPos = pos(x, y1);
  uint16_t endPos = pos(x, y2);
  
  if (startPos == endPos) // On same buffer location
  {
    rtn = drawBits(startPos, (y1 % 8), (y2 % 8), mode);
    if (_directDraw) showBuffer(startPos, 1);
    return rtn;
  }
  for (int i = startPos; i <= endPos; i += 128)
  {
    if (i == startPos) 
      rtn += drawBits(i, (y1 % 8), 7, mode);      
    else if (i == endPos) 
      rtn += drawBits(i, 0, (y2 % 8), mode);      
    else rtn += drawBits(i, 0, 7, mode);

    if (_directDraw) showBuffer(i, 1);
  }  
  return rtn;    
}

uint16_t OLED12864::drawHLine(uint8_t x1, uint8_t x2, uint8_t y, uint8_t mode)
{

  if (!_enableBuffer) return 0;

  int rtn = 0;
  
  if (x1 == x2) return drawPixel(x1, y, mode);
  
  if (x1 > x2) swap(x1, x2);
  
  uint16_t startPos = pos(x1, y);
  uint16_t endPos = pos(x2, y);

  uint8_t drawPattern = _BV((y % 8));

  for (int i = startPos; i <= endPos; i++)
    rtn += drawBuffer(i, drawPattern, mode, 1);

  if (_directDraw) {
    showBuffer(startPos, endPos - startPos + 1);
  }

  return rtn;    
}

uint16_t OLED12864::drawHBits(uint8_t x1, uint8_t x2, uint8_t line, uint8_t drawPattern, uint8_t mode)
{

  if (!_enableBuffer) return 0;

  int rtn = 0;
  
  // if (x1 == x2) return drawPixel(x1, y, mode);
  
  if (x1 > x2) swap(x1, x2);
  
  uint16_t startPos = posXL(x1, line);
  uint16_t endPos = posXL(x2, line);

  for (int i = startPos; i <= endPos; i++)
    rtn += drawBuffer(i, drawPattern, mode, 1);

  if (_directDraw) {
    showBuffer(startPos, endPos - startPos + 1);
  }

  return rtn;    
}

void OLED12864::showBuffer(uint16_t start_pos, uint16_t byte_cnt)
{
  if (!_enableBuffer) return;

  uint8_t line = pos2line(start_pos);
  uint8_t start = pos2x(start_pos);
  byte col = start + _colOffset;

  if (_i2c)
  {
    switch (_chipset) 
    {

      case OLED_1106i2c:

        setDisplayPos(start, line);
        i2cSendBuffer(start_pos, byte_cnt);
        break;

      case OLED_1306i2c:

        // direct clear screen
        sendCommand(OLED_CMD_1306_ADDRESSMODE);           // 0x20
        sendCommand(OLED_HORIZONTAL_ADDRESSING);          // 0x00 - Good for filling buffers

        setDisplayPos(start, line);
        i2cSendBuffer(start_pos, byte_cnt);
        break;

    }
  } 
}


uint16_t OLED12864::drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode, uint8_t style)
{

  if (!_enableBuffer) return 0;

  uint8_t line;
  uint16_t startPos = pos(x1, y1);
  uint16_t endPos = pos(x1, y2);
  uint8_t drawPattern = 0;

  int rtn = 0;
  if (x1 == x2) return drawVLine(x1, y1, y2, mode);
  if (y1 == y2) return drawHLine(x1, x2, y1, mode);
  
  if (x1 > x2) swap(x1, x2);
  if (y1 > y2) swap(y1, y2);


  if (y2 - y1 > 1) // draw horizontal line only if rectangle with width > 2
  {
    switch (style)
    {
      case 0:
        rtn += drawVLine(x1, y1, y2, mode);
        rtn += drawVLine(x2, y1, y2, mode);
        rtn += drawHLine(x1+1, x2-1, y1, mode);
        rtn += drawHLine(x1+1, x2-1, y2, mode);
        break;
      case 1:
                
        for (int i = startPos; i <= endPos; i += 128)
        {
          line = pos2line(i);
          drawPattern = 0;
          if (i == startPos) {
            if (i == endPos) {
              for (uint8_t idx = (y1 % 8); idx <= (y2 % 8); idx++) drawPattern |= _BV(idx);
            } else {    
              for (uint8_t idx = (y1 % 8); idx <= 7; idx++) drawPattern |= _BV(idx);
            }    
          } else if (i == endPos) {
            for (uint8_t idx = 0; idx <= (y2 % 8); idx++) drawPattern |= _BV(idx);
          } else {
            drawPattern = 0xFF;
          }    
          rtn += drawHBits(x1, x2, line, drawPattern, mode);
        }  
        break;
      default:
        break;
    }
  }
  return rtn;
}


