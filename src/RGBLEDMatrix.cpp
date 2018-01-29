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
#if TWELVE_BIT_COLOR
#define MAX_SCAN_PASS_COUNT 15
#else
#define MAX_SCAN_PASS_COUNT 3
#endif

const unsigned long UPDATE_INTERVAL = 2000;

RGBLEDMatrix::RGBLEDMatrix( 
	int rows,
	int columns,
	RGBLEDBitLayout bitLayout,
	bool columnControlBitOn,
	bool rowControlBitOn,
	unsigned int interFrameOffTimeMicros,
	int slavePin		
) :		BaseLEDMatrix(
				rows,
				columns,
				3,
				MAX_SCAN_PASS_COUNT,
				columnControlBitOn,
				rowControlBitOn,
				interFrameOffTimeMicros,
				slavePin
			),
		_bitLayout(bitLayout),
		_screen_data(NULL)
{

}

void RGBLEDMatrix::setup() {
	this->BaseLEDMatrix::setup();
	
	if (_screen_data == NULL) {
		_screen_data = new MutableRGBImage(this->rows(), this->columns());
	}
}

RGBLEDMatrix::~RGBLEDMatrix() {
	if (_screen_data != NULL) {
		delete _screen_data;
	}
}

bool RGBLEDMatrix::matrixNeedsUpdate(void) const {
	return this->image().isDirty();
}
void RGBLEDMatrix::matrixHasBeenUpdated(void) {
	this->image().setNotDirty();
}

void RGBLEDMatrix::generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const {
	for (unsigned int row = 0; row < this->rows(); row++) {
		this->setRowBitsForFrame(row, frame, frameBits, *_screen_data);
	}
}

size_t RGBLEDMatrix::maxFrameCountForValue(RGBColorType value) const {
#if TWELVE_BIT_COLOR
	switch (value) {
		case 0:
			return 0;
			break;
		case 0x0100:
		case 0x0010:
		case 0x0001:
			return 1;
			break;
		case 0x0200:
		case 0x0020:
		case 0x0002:
			return 2;
			break;
		case 0x0300:
		case 0x0030:
		case 0x0003:
			return 3;
			break;
		case 0x0400:
		case 0x0040:
		case 0x0004:
			return 4;
			break;
		case 0x0500:
		case 0x0050:
		case 0x0005:
			return 5;
			break;
		case 0x0600:
		case 0x0060:
		case 0x0006:
			return 6;
			break;
		case 0x0700:
		case 0x0070:
		case 0x0007:
			return 7;
			break;
		case 0x0800:
		case 0x0080:
		case 0x0008:
			return 8;
			break;
		case 0x0900:
		case 0x0090:
		case 0x0009:
			return 9;
			break;
		case 0x0A00:
		case 0x00A0:
		case 0x000A:
			return 10;
			break;
		case 0x0B00:
		case 0x00B0:
		case 0x000B:
			return 11;
			break;
		case 0x0C00:
		case 0x00C0:
		case 0x000C:
			return 12;
			break;
		case 0x0D00:
		case 0x00D0:
		case 0x000D:
			return 13;
			break;
		case 0x0E00:
		case 0x00E0:
		case 0x000E:
			return 14;
			break;
		case 0x0F00:
		case 0x00F0:
		case 0x000F:
			return 15;
			break;
		default:
			return MAX_SCAN_PASS_COUNT;
			break;
	}
#else
	// we expect only on of the 4 bit sets in the value to be on
	switch (value) {
		case 0:
			return 0;
			break;
		case B00010000:
		case B00000100:
		case B00000001:
			return 1;
			break;
		case B00100000:
		case B00001000:
		case B00000010:		
			return 2;
			break;
		case B00110000:
		case B00001100:
		case B00000011:		
		default:
			return MAX_SCAN_PASS_COUNT;
			break;
	}
#endif
}

void RGBLEDMatrix::setRowBitsForFrame(
	int row,
	size_t frame,
	LEDMatrixBits& frameBits,
	const MutableRGBImage& image ) const 
{	
	if (!frameBits.isRowMemoized(row)) {
		bool rowNeedsPower = false;
		size_t colBitIdx = 0;
		size_t redBitOffset = 0;
		size_t greenBitOffset = 1;
		size_t blueBitOffset = 2;
		size_t columnBitIdxIncrement = 3;
		if (_bitLayout == RGB_GROUPS) {
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
		for (unsigned int col = 0; col < this->columns(); col++) {
			RGBColorType rgbValue = image.pixel(row, col);

			// a form of Binary Code Modulation is used to control
			// the LED intensity at variou levels.
		
			// red
			RGBColorType redValue = rgbValue & RED_MASK;
			if (redValue && frame <= RGBLEDMatrix::maxFrameCountForValue(redValue) ) {
				frameBits.setColumnControlBit(row,colBitIdx+redBitOffset,true);
				rowNeedsPower = true;
			}
			
			// green
			RGBColorType greenValue = rgbValue & GREEN_MASK;
			if (greenValue && frame <= RGBLEDMatrix::maxFrameCountForValue(greenValue) ) {
				frameBits.setColumnControlBit(row,colBitIdx+greenBitOffset,true);
				rowNeedsPower = true;
			}
					
			// blue
			RGBColorType blueValue = (rgbValue & BLUE_MASK);
			if (blueValue && frame <= RGBLEDMatrix::maxFrameCountForValue(blueValue) ) {
				frameBits.setColumnControlBit(row,colBitIdx+blueBitOffset,true);
				rowNeedsPower = true;
			}
			colBitIdx += columnBitIdxIncrement;
			
		}		
		frameBits.setRowControlBit(row,rowNeedsPower);
	} 
}


// Number of 5 microsecond units
unsigned int RGBLEDMatrix::baseIntervalMultiplier( size_t frame ) const {
#if TWELVE_BIT_COLOR
#if defined(__AVR_ATmega2560__)||defined(__AVR_ATmega1284__)||defined(__AVR_ATmega1284P__)
	unsigned int multiplier = frame/8 + 1;
#else
	unsigned int multiplier = frame/4 + 1;
#endif

#else
	unsigned int multiplier = 1;
	switch (frame) {
		case 2:
			multiplier = 3;
			break;
		case 3:
			multiplier = 8;
			break;
	}
#endif
	
	return  multiplier;
}
