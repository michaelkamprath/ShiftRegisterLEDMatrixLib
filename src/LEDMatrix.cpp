//     Shift Register LED Matrix Project - LEDMatrix
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
#include "LEDMatrix.h"

#define LED_MATRIX_MAX_SCAN_PASS_COUNT 1

LEDMatrix::LEDMatrix( 
	uint16_t rows,
	uint16_t columns,
	bool columnControlBitOn,
	bool rowControlBitOn,
	unsigned int interFrameOffTimeMicros,
	int slavePin
) :		BaseLEDMatrix(
				rows,
				columns,
				rows,
				columns,
				1,
				LED_MATRIX_MAX_SCAN_PASS_COUNT,
				columnControlBitOn,
				rowControlBitOn,
				interFrameOffTimeMicros,
				slavePin
			),
		GFXcanvas1(columns, rows),
		_matrixNeedsUpdate(false)
{

}

void LEDMatrix::setup() {
	this->BaseLEDMatrix::setup();
}

LEDMatrix::~LEDMatrix() {
}

bool LEDMatrix::matrixNeedsUpdate(void) const {
	return _matrixNeedsUpdate;
}
void LEDMatrix::matrixHasBeenUpdated(void) {
	_matrixNeedsUpdate = false;
}

void LEDMatrix::generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const {
	for (uint16_t row = 0; row < this->rows(); row++) {
		this->setRowBitsForFrame(row, frame, frameBits);
	}
}

void LEDMatrix::setRowBitsForFrame(
	uint16_t row,
	size_t frame,
	LEDMatrixBits& frameBits
) const 
{	
	if (!frameBits.isRowMemoized(row)) {
		bool rowNeedsPower = false;
		size_t colBitIdx = 0;
		for (uint16_t col = 0; col < this->columns(); col++) {
			uint16_t pixel = this->rawPixel(col, row);
			
			if (pixel > 0) {
				frameBits.setColumnControlBit(row,colBitIdx,true);
				rowNeedsPower = true;
			}
			
			colBitIdx++;			
		}		
		frameBits.setRowControlBit(row,rowNeedsPower);
	}	
}

ICACHE_RAM_ATTR unsigned int LEDMatrix::baseIntervalMultiplier( size_t frame ) const {
	return  10;
}

#pragma mark - Adafruit GFX Support

uint16_t LEDMatrix::rawPixel( int16_t x, int16_t y ) const {
	if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return 0;
	if(this->getBuffer()) {
		uint8_t *buffer = this->getBuffer();
		uint8_t *ptr  = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];
		
		return ((*ptr) & (0x80 >> (x & 7))) != 0;
	}
	return 0;
}

uint16_t LEDMatrix::pixel( int16_t x, int16_t y ) const {
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

void LEDMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
	this->GFXcanvas1::drawPixel(x, y, color);
	_matrixNeedsUpdate = true;
}

void LEDMatrix::fillScreen(uint16_t color) {
	this->GFXcanvas1::fillScreen(color);
	_matrixNeedsUpdate = true;
}
