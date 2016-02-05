#include <Wire.h>
#include <avr/pgmspace.h>
#include <OLED12864.h>


boolean OLED12864::plotter(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax, int16_t xAxis, int16_t yAxis, boolean frame)
{
  if (!_enableBuffer) return false;
	if ((xMin >= xMax) || (yMin >= yMax)) 
	{
		_plotter.ready = false;
		return false;
	}

	_plotter.frame = frame;
	_plotter.newDraw = true;
	_plotter.xMin = xMin;
	_plotter.xMax = xMax;
	_plotter.xAxis = xAxis;
	_plotter.yMin = yMin;
	_plotter.yMax = yMax;
	_plotter.yAxis = yAxis;
	_plotter.xMinPos = 0 + (frame ? 2 : 0);
	_plotter.xMaxPos = 127 - (frame ? 2 : 0);
	_plotter.yMinPos = 63 - (frame ? 2 : 0);
	_plotter.yMaxPos = 0 + (frame ? 2 : 0);
	_plotter.xLast = xAxis;
	_plotter.yLast = yAxis;
	_plotter.ready = true;
	plotterClear();
	return true;
}

void OLED12864::plotterClear() 
{
  if ((!_enableBuffer) || (!_plotter.ready)) return;
  clr();
  if (_plotter.frame) drawRect(0,0,127,63,0, 0);
  if ((_plotter.xMin <= _plotter.xAxis) && (_plotter.xAxis <= _plotter.xMax))
  {
  	uint8_t xPos = map(_plotter.xAxis, _plotter.xMin, _plotter.xMax, _plotter.xMinPos, _plotter.xMaxPos);
  	drawVLine(xPos, _plotter.yMinPos, _plotter.yMaxPos, 0);
  }
  if ((_plotter.yMin <= _plotter.yAxis) && (_plotter.yAxis <= _plotter.yMax))
  {
  	uint8_t yPos = map(_plotter.yAxis, _plotter.yMin, _plotter.yMax, _plotter.yMinPos, _plotter.yMaxPos);
  	drawHLine(_plotter.xMinPos, _plotter.xMaxPos, yPos, 0);
  }
	return;
}

boolean OLED12864::plotterDraw(int16_t x, int16_t y) 
{
  if ((!_enableBuffer) || (!_plotter.ready)) return false;

	// Update last pos even not on screen, it can be used for line
  _plotter.xLast = x;
  _plotter.yLast = y;
  _plotter.newDraw = false;

  if ((x < _plotter.xMin) || (x > _plotter.xMax) || (y < _plotter.yMin) || (y > _plotter.yMax)) return false;

	uint8_t xPos, yPos;
	xPos = getXPos(x);
	yPos = getYPos(y);

  drawPixel(xPos, yPos, 0);

	return true;
}


int16_t OLED12864::getXPos(int16_t x) 
{
	return  map(x,_plotter.xMin, _plotter.xMax, _plotter.xMinPos, _plotter.xMaxPos);
}

int16_t OLED12864::getYPos(int16_t y) 
{
	return  map(y,  _plotter.yMin, _plotter.yMax, _plotter.yMinPos, _plotter.yMaxPos);
}



boolean OLED12864::plotterDrawTo(int16_t x, int16_t y) 
{

  if ((!_enableBuffer) || (!_plotter.ready)) return false;
 
	// allow showing the line outside the screen (but it's not recommanded)  
  // Check if any potion of the line can be see on screen, otherwise, just mark the point
  // line from (_plotter.xLast, _plotter.yLast)	 to (x, y)
  //	(1) any of these two points inside the display range
  //	(2) middle potion of the line is inside the display range
  //		  (a) the rectange of these two points overrlapped with the display range
  //      (b) check the boundary of display area hit the line
  //                                (x - x1)
  //            y = y1 + (y2 - y1) ------------
  //                                (x2 - x1)  
  //
  //  If so, get the displayed line potion

  int x1, x2, y1, y2;

  if (_plotter.newDraw) plotterDraw(x, y);

  // Check x from left to right
  if (_plotter.xLast <= x) {
  	x1 = _plotter.xLast;
  	x2 = x;
  	y1 = _plotter.yLast;
  	y2 = y;
  } else {
  	x1 = x;
  	x2 = _plotter.xLast;
  	y1 = y;
  	y2 = _plotter.yLast;
  }

  // key the original x, y for better drawing later
  _plotter.xLast = x;
  _plotter.yLast = y;
	_plotter.newDraw = false;

  // out of range
  if (((x1 < _plotter.xMin) && (x2 < _plotter.xMin)) || ((x1 > _plotter.xMax) && (x2 > _plotter.xMax)) ||
			((y1 < _plotter.yMin) && (y2 < _plotter.yMin)) || ((y1 > _plotter.yMax) && (y2 > _plotter.yMax))) 
  {	
  	// out of range, nothing to do, just record the new point
  	return false;
  }	

  // use screen coordinate for calculation
  // - just for fast calcualtion on display, may not be good for all situation (e.g. the case with reduced resolution)
  x1 = getXPos(x1);
  y1 = getYPos(y1);
  x2 = getXPos(x2);
  y2 = getYPos(y2);

	if (x1 == x2) {
		if (y1 > y2) swap(y1, y2);
		// vertical line, alreay checked it should be in range, just check if the line is being cut
		// note, y is in reverse position
		if (y1 < _plotter.yMaxPos) y1 = _plotter.yMaxPos;
		if (y2 > _plotter.yMinPos) y2 = _plotter.yMinPos;
		drawVLine(x1, y1, y2, 0);
		return true;
  }

	if (y1 == y2) {
		// horizontal line, alreay checked it should be in range, just check if the line is being cut
		if (x1 < _plotter.xMinPos) x1 = _plotter.xMinPos;
		if (x2 > _plotter.xMaxPos) x2 = _plotter.xMaxPos;
		drawHLine(x1, x2, y1, 0);
		return true;
  }

  plotterDrawLine(x1, y1, x2, y2);
  return true;

}

// *** Need to consider the point outside the display area, so it must be int16_t instead of uint8_t
void OLED12864::plotterDrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  int16_t ex, ey;
	if (abs(x1 - x2) >= abs(y1 - y2)) {

		if (x1 > x2) {
			swap(x1, x2);
			swap(y1, y2);
		}

		// scan for X-axis
		if (x1 < _plotter.xMinPos) {
			y1 = y1 + (y2 - y1) * (_plotter.xMinPos - x1) / (x2 - x1);
			x1 = _plotter.xMinPos;
		}
		if (x2 > _plotter.xMaxPos) {
			y2 = y1 + (y2 - y1) * (_plotter.xMaxPos - x1) / (x2 - x1);
			x2 = _plotter.xMaxPos;
		}

		for (uint8_t ex = x1; ex < x2; ex++) {
			ey = y1 + (y2 - y1) * (ex - x1) / (x2 - x1);
			if (plotterValidY(ey)) drawPixel(ex, ey, 0);
		}

	} else {

		// scan for y-axis
		if (y1 > y2) {
			swap(x1, x2);
			swap(y1, y2);
		}


		// scan for X-axis
		if (y1 < _plotter.yMaxPos) {
			x1 = x1 + (x2 - x1) * (_plotter.yMaxPos - y1) / (y2 - y1);
			y1 = _plotter.yMaxPos;
		}
		if (y2 > _plotter.yMinPos) {
			x2 = x1 + (x2 - x1) * (_plotter.yMinPos - y1) / (y2 - y1);
			y2 = _plotter.yMinPos;
		}

		for (int16_t ey = y1; ey < y2; ey++) {
			ex = x1 + (x2 - x1) * (ey - y1) / (y2 - y1);
			if (plotterValidX(ex)) drawPixel(ex, ey, 0);
		}


	}
}

