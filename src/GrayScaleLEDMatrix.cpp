//     Shift Register LED Matrix Project - GrayScaleLEDMatrix
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
#include "GrayScaleLEDMatrix.h"

#define LED_MATRIX_MAX_SCAN_PASS_COUNT 1

GrayScaleLEDMatrix::GrayScaleLEDMatrix( 
	int rows,
	int columns,
	bool columnControlBitOn,
	bool rowControlBitOn,
	unsigned int interFrameOffTimeMicros,
	int slavePin
) :		BaseLEDMatrix(
				rows,
				columns,
				1,
				3,
				columnControlBitOn,
				rowControlBitOn,
				interFrameOffTimeMicros,
				slavePin
			),
		_screen_data(NULL)
{

}

void GrayScaleLEDMatrix::setup() {
	this->BaseLEDMatrix::setup();
	
	if (_screen_data == NULL) {
		_screen_data = new MutableGrayScaleImage(this->rows(), this->columns());
	}
}

GrayScaleLEDMatrix::~GrayScaleLEDMatrix() {
	if (_screen_data != NULL) {
		delete _screen_data;
	}
}

bool GrayScaleLEDMatrix::matrixNeedsUpdate(void) const {
	return this->image().isDirty();
}
void GrayScaleLEDMatrix::matrixHasBeenUpdated(void) {
	this->image().setNotDirty();
}

void GrayScaleLEDMatrix::generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const {
	for (unsigned int row = 0; row < this->rows(); row++) {
		this->setRowBitsForFrame(row, frame, frameBits, *_screen_data);
	}
}

void GrayScaleLEDMatrix::setRowBitsForFrame(
	int row,
	size_t frame,
	LEDMatrixBits& frameBits,
	const LEDImageBase<GrayScaleColorType>& image ) const 
{	
	if (!frameBits.isRowMemoized(row)) {
		bool rowNeedsPower = false;
		size_t colBitIdx = 0;
		for (unsigned int col = 0; col < this->columns(); col++) {
			GrayScaleColorType color = image.pixel(row, col);
			
			if (color >= frame) {
				frameBits.setColumnControlBit(row,colBitIdx,true);
				rowNeedsPower = true;
			}
			
			colBitIdx++;			
		}		
		frameBits.setRowControlBit(row,rowNeedsPower);
	}	
}

ICACHE_RAM_ATTR unsigned int GrayScaleLEDMatrix::baseIntervalMultiplier( size_t frame ) const {
	
	return frame*frame;
}

