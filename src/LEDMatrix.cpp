//     RGB Matrix Project - LEDMatrix
//     Copyright (C) 2017 Michael Kamprath
//
//     This file is part of RGB Matrix Project.
// 
//     RGB Matrix Project is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     RGB Matrix Project is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with RGB Matrix Project.  If not, see <http://www.gnu.org/licenses/>.

#include <Arduino.h>
#include "LEDMatrix.h"

#define LED_MATRIX_MAX_SCAN_PASS_COUNT 1

LEDMatrix::LEDMatrix( 
	int rows,
	int columns,
	bool columnControlBitOn,
	bool rowControlBitOn,
	int slavePin
) :		BaseLEDMatrix(
				rows,
				columns,
				1,
				LED_MATRIX_MAX_SCAN_PASS_COUNT,
				columnControlBitOn,
				rowControlBitOn,
				slavePin
			),
		_screen_data(NULL)
{

}

void LEDMatrix::setup() {
	this->BaseLEDMatrix::setup();
	
	if (_screen_data == NULL) {
		_screen_data = new MutableGlyph(this->rows(), this->columns());
	}
}

LEDMatrix::~LEDMatrix() {
	if (_screen_data != NULL) {
		delete _screen_data;
	}
}

bool LEDMatrix::matrixNeedsUpdate(void) const {
	return this->image().isDirty();
}
void LEDMatrix::matrixHasBeenUpdated(void) {
	this->image().setNotDirty();
}

void LEDMatrix::generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const {
	for (unsigned int row = 0; row < this->rows(); row++) {
		this->setRowBitsForFrame(row, frame, frameBits, *_screen_data);
	}
}

void LEDMatrix::setRowBitsForFrame(
	int row,
	size_t frame,
	LEDMatrixBits& frameBits,
	const GlyphBase& image ) const 
{	
	if (!frameBits.isRowMemoized(row)) {
		bool rowNeedsPower = false;
		size_t colBitIdx = 0;
		for (unsigned int col = 0; col < this->columns(); col++) {
			bool isOn = image.getBit(row, col);
			
			if (isOn) {
				frameBits.setColumnControlBit(row,colBitIdx,true);
				rowNeedsPower = true;
			}
			
			colBitIdx++;			
		}		
		frameBits.setRowControlBit(row,rowNeedsPower);
	}	
}
