//     Shift Register LED Matrix Project - RGBImage
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
#include "RGBImage.h"
#include "Glyph.h"

#pragma mark RGBImage

RGBImage::RGBImage(
	int rows,
	int columns,
	const RGBColorType* data,
	bool isFromProgramSpace )
:	LEDImage<RGBColorType>( rows, columns, data, isFromProgramSpace )
{
}

RGBImage::RGBImage(const LEDImageBase<RGBColorType>& other)
:	LEDImage<RGBColorType>(other)
{
}

RGBImage::~RGBImage()
{
}

#pragma mark MutableRGBImage

MutableRGBImage::MutableRGBImage( int rows, int columns )
:	MutableLEDImage<RGBColorType,BLACK_COLOR,TRANSPARENT_COLOR>( rows, columns )
{
}
	
MutableRGBImage::MutableRGBImage(
	int rows,
	int columns,
	const RGBColorType* data,
	bool isFromProgramSpace )
:	MutableLEDImage<RGBColorType,BLACK_COLOR,TRANSPARENT_COLOR>( rows, columns, data, isFromProgramSpace )
{
}

MutableRGBImage::MutableRGBImage(const LEDImageBase<RGBColorType>& other)
:	MutableLEDImage<RGBColorType,BLACK_COLOR,TRANSPARENT_COLOR>(other)
{
}

MutableRGBImage::~MutableRGBImage()
{
}

void MutableRGBImage::drawGlyph(
		const LEDImageBase<bool>& glyph,
		int row,
		int column,
		RGBColorType foreground,
		RGBColorType background
	)
{
	if (	row >= this->rows()
			|| column >= this->columns()
			|| (glyph.columns() + column <= 0 )
			|| (glyph.rows() + row <= 0) 
		) {
		return;
	}

	this->makeDirty();
	int imageX = 0;
	int thisX = column;
	if (column < 0) {
		imageX = -column;
		thisX = 0;
	}
	
	int imageColumns = glyph.columns() - imageX;
	int thisColumns = column <= 0 ? this->columns() : this->columns() - column;
	if (imageColumns > thisColumns) {
		imageColumns = thisColumns;
	}

	int imageY = 0;
	int startRow = row;
	if (row < 0) {
		imageY = -row;
		startRow = 0;
	}

	for (	
			int yT = startRow, yGlyph = imageY;
			(yT < this->rows()) && (yGlyph < glyph.rows()); 
			yT++, yGlyph++ 
		) { 
		for (int colCounter = 0; colCounter < imageColumns; colCounter++) {
			if (glyph.pixel(yGlyph,imageX+colCounter)) {
				this->pixel(yT,thisX+colCounter) = foreground;
			}
			else if ( background != TRANSPARENT_COLOR ) {
				this->pixel(yT,thisX+colCounter) = background;
			}
		}
	}
}
