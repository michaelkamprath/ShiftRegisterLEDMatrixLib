//     Shift Register LED Matrix Project - RGBLEDMatrix
//     Copyright (C) 2017 Michael Kamprath
//
//     This file is part of Shift Register LED Matrix Project.
// 
//     Shift Register LED Matrix Project is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     Shift Register LED Matrix Project is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.

#include <Arduino.h>
#include "RGBLEDMatrix.h"

// #define SPICACHEPADBITS(rows,cols)	(rows*3 + cols)%8 ? 8 - (rows*3 + cols)%8 : 0
// #define SPICACHESIZE(rows,cols)	1+((rows*3 + cols)-1)/8

#if SIXTEEN_BIT_COLOR
#define MAX_SCAN_PASS_COUNT 6
#else
#define MAX_SCAN_PASS_COUNT 3
#endif

const unsigned long UPDATE_INTERVAL = 2000;

#define CONTROL_ROWS( rows, columns, layout)	\
	(layout == RGBLEDMatrix::RGB_GROUPS_CPRG8) ? 8 : rows

#define CONTROL_COLUMNS( rows, columns, layout)	\
	(layout == RGBLEDMatrix::RGB_GROUPS_CPRG8) ? (rows/8)*columns : columns

RGBLEDMatrix::RGBLEDMatrix( 
	int rows,
	int columns,
	RGBLEDBitLayout bitLayout,
	bool columnControlBitOn,
	bool rowControlBitOn,
	unsigned int interFrameOffTimeMicros,
	int slavePin,
	DeviceBitEndian bitEndian	
) :		BaseLEDMatrix(
				rows,
				columns,
				CONTROL_ROWS( rows, columns, bitLayout),
				CONTROL_COLUMNS( rows, columns, bitLayout),
				3,
				MAX_SCAN_PASS_COUNT,
				columnControlBitOn,
				rowControlBitOn,
				interFrameOffTimeMicros,
				slavePin,
				bitEndian
			),
		GFXcanvas16(columns, rows),
		_bitLayout(bitLayout),
		_matrixNeedsUpdate(false)
{

}

void RGBLEDMatrix::setup() {
	this->BaseLEDMatrix::setup();
}

RGBLEDMatrix::~RGBLEDMatrix() {
}

bool RGBLEDMatrix::matrixNeedsUpdate(void) const {
	return _matrixNeedsUpdate;
}
void RGBLEDMatrix::matrixHasBeenUpdated(void) {
	_matrixNeedsUpdate = false;
}


bool RGBLEDMatrix::allowedFrameForValue(uint16_t value, size_t frame) const {
#if SIXTEEN_BIT_COLOR
	switch (frame) {
		case 0:
			// bit 0 of each of RGB
			return (value&0x0821) != 0;
			break;
		case 1:
			// green has a bit specific to this pass, on for that.
			return ( (value&0x0040) != 0 ) ||
			// only one for red and blue if both the prior pass and next pass are on
					(value&0x1800 == 0x1800) || (value&0x0003 == 0x0003);
				break;
		case 2:
			// red bit 2, green bit 3, blue bit 2
			return (value&0x1082) != 0;
			break;
		case 3:
			return (value&0x2104) != 0;
			break;
		case 4:
			return (value&0x4208) != 0;
			break;
		case 5:
			return (value&0x8410) != 0;
			break;
		default:
			return false;
	}
#else
	switch (frame) {
		case 0:
			// bit 0 of each of RGB
			return (value&0x31E6) != 0;
			break;
		case 1:
			return (value&0x4208) != 0;
			break;
		case 2:
			return (value&0x8410) != 0;
			break;
		default:
			return false;
			break;
	}
#endif
}

void RGBLEDMatrix::generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const {
	for (unsigned int row = 0; row < this->controlRows(); row++) {
		this->setControlRowBitsForFrame(row, frame, frameBits);
	}
}

void RGBLEDMatrix::setControlRowBitsForFrame(
	unsigned int controlRow,
	size_t frame,
	LEDMatrixBits& frameBits ) const 
{	
	if (!frameBits.isRowMemoized(controlRow)) {
		bool rowNeedsPower = false;
		size_t redBitOffset = 0;
		size_t greenBitOffset = 1;
		size_t blueBitOffset = 2;
		size_t columnBitIdxIncrement = 3;
		if (_bitLayout == RGB_GROUPS || _bitLayout == RGB_GROUPS_CPRG8) {
			redBitOffset = 0;
			greenBitOffset = this->columns();
			blueBitOffset = 2*this->columns();
			columnBitIdxIncrement = 1;
		} else if (_bitLayout == RBG_GROUPS) {
			redBitOffset = 0;
			greenBitOffset = 2*this->columns();
			blueBitOffset = this->columns();
			columnBitIdxIncrement = 1;
		}
		
		for (unsigned int rowGroup = 0; rowGroup < this->rowGroups(); rowGroup++) {
			rowNeedsPower |= this->setColumnBitsForControlRowAndFrame(
				controlRow,
				rowGroup,
				redBitOffset,
				greenBitOffset,
				blueBitOffset,
				columnBitIdxIncrement,
				frame,
				frameBits
			);
		}		
		frameBits.setRowControlBit(controlRow,rowNeedsPower);
	} 
}

bool RGBLEDMatrix::setColumnBitsForControlRowAndFrame(
	unsigned int controlRow,
	unsigned int rowGroup,
	size_t redBitOffset,
	size_t greenBitOffset,
	size_t blueBitOffset,
	size_t columnBitIdxIncrement,
	size_t frame,
	LEDMatrixBits& frameBits ) const 
{
	bool rowNeedsPower = false;	
	size_t colBitIdx = rowGroup*this->columns()*this->columnBitWidth();
	unsigned int imageRow = (this->rowGroups() - 1 - rowGroup)*this->controlRows() + controlRow;

	for (unsigned int col = 0; col < this->columns(); col++) {
		unsigned int endianCol = col;
		
		if (this->bitEndian() == LED_LITTLE_ENDIAN_8 ) {
			// first get the column "byte"
			unsigned int colByte = col/8;

			// now column's "but" in the byte
			unsigned int colBit = col%8;
			
			// calculate new column location by on "byte"
			
			endianCol = this->columns() - (colByte+1)*8 + colBit;
		}
		
		RGBColorType rgbValue = this->rawPixel(endianCol, imageRow);
		// a form of Binary Code Modulation is used to control
		// the LED intensity at variou levels.
	
		// red
		RGBColorType redValue = rgbValue & RED_COLOR_MASK;
		if (redValue && RGBLEDMatrix::allowedFrameForValue(redValue, frame) ) {
			frameBits.setColumnControlBit(controlRow,colBitIdx+redBitOffset,true);
			rowNeedsPower = true;
		}
		
		// green
		RGBColorType greenValue = rgbValue & GREEN_COLOR_MASK;
		if (greenValue && RGBLEDMatrix::allowedFrameForValue(greenValue, frame) ) {
			frameBits.setColumnControlBit(controlRow,colBitIdx+greenBitOffset,true);
			rowNeedsPower = true;
		}
				
		// blue
		RGBColorType blueValue = (rgbValue & BLUE_COLOR_MASK);
		if (blueValue && RGBLEDMatrix::allowedFrameForValue(blueValue, frame) ) {
			frameBits.setColumnControlBit(controlRow,colBitIdx+blueBitOffset,true);
			rowNeedsPower = true;
		}
		colBitIdx += columnBitIdxIncrement;		
	}
	
	return 	rowNeedsPower;
}

// Number of 5 microsecond units
unsigned int RGBLEDMatrix::baseIntervalMultiplier( size_t frame ) const {
	return (1 << frame);
}

#pragma mark - Adafruit GFX Support

uint16_t RGBLEDMatrix::rawPixel( int16_t x, int16_t y ) const {
	if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return 0;
	if(this->getBuffer()) {
		return this->getBuffer()[x + y * this->WIDTH];
	}
	return 0;
}

uint16_t RGBLEDMatrix::pixel( int16_t x, int16_t y ) const {
	int16_t t;
	switch(this->getRotation()) {
		case 1:
			t = x;
			x = WIDTH  - 1 - y;
			y = t;
			break;
		case 2:
			x = WIDTH  - 1 - x;
			y = HEIGHT - 1 - y;
			break;
		case 3:
			t = x;
			x = y;
			y = HEIGHT - 1 - t;
			break;
	}	
	return this->rawPixel(x, y);
}

void RGBLEDMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
	this->GFXcanvas16::drawPixel(x, y, color);
	_matrixNeedsUpdate = true;
}

void RGBLEDMatrix::fillScreen(uint16_t color) {
	this->GFXcanvas16::fillScreen(color);
	_matrixNeedsUpdate = true;
}

#pragma mark - Debugging

void RGBLEDMatrix::debugPrintImageData(void) const {
	char c[8];
	Serial.println("Current image data:");
	for (uint16_t y = 0; y < this->HEIGHT; y++ ) {
		Serial.print("    ");
		for (uint16_t x = 0; x < this->WIDTH; x++) {
			Serial.print("0x");
			sprintf(c, "%04x", this->rawPixel(x,y));
			Serial.print(c);
			Serial.print(" ");
		}
		Serial.print("\n");
	}
}
