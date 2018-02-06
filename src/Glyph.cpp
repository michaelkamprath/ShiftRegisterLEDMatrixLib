//     Shift Register LED Matrix Project - Glyph
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
#if defined ( ESP8266 )
  #include <pgmspace.h>
#else
  #include <avr/pgmspace.h>
#endif
#include "Glyph.h"
#include "SRLEDMatrixUtils.h"

// pre-calculate the bit masks to speed up over 1<<(7-bitIdx%8)
const unsigned char BIT_MASKS[8] = {
		B10000000,
		B01000000,
		B00100000,
		B00010000,
		B00001000,
		B00000100,
		B00000010,
		B00000001
	};

#pragma mark GrayScaleImage

Glyph::Glyph(
	int rows,
	int columns,
	const bool* data,
	bool isFromProgramSpace )
:	LEDImage<bool>( rows, columns, data, isFromProgramSpace )
{
}

Glyph::Glyph(const LEDImageBase<bool>& other)
:	LEDImage<bool>(other)
{
}

Glyph::~Glyph()
{
}

bool* Glyph::generateBitBoolArray( 
	int rows,
	int columns,
	const unsigned char* data,
	bool isFromProgramSpace
) {
	bool* boolArray = new bool[rows*columns];
	
	if ( data != NULL ) {
		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < columns; x++ ) {
				int bitIdx = y*columns + x;
				unsigned char dataByte = 0;
				if (isFromProgramSpace) {
					dataByte = pgm_read_byte_near(data + bitIdx/8);
				} else {
					dataByte = data[bitIdx/8];
				}
				
				boolArray[bitIdx] = ( ( dataByte & BIT_MASKS[bitIdx%8] ) != 0 );
			}
		}
	} else {
		// zero out array
		memset(boolArray, 0, rows*columns);
	}
	
	return boolArray;
}

#pragma mark MutableGlyph

MutableGlyph::MutableGlyph( int rows, int columns )
:	MutableLEDImage<bool,false,0x80>( rows, columns )
{
}
	
MutableGlyph::MutableGlyph(
	int rows,
	int columns,
	const bool* data,
	bool isFromProgramSpace )
:	MutableLEDImage<bool,false,0x80>( rows, columns, data, isFromProgramSpace )
{
}

MutableGlyph::MutableGlyph(const LEDImageBase<bool>& other)
:	MutableLEDImage<bool,false,0x80>(other)
{
}

MutableGlyph::~MutableGlyph()
{
}

void MutableGlyph::placeGlyphAt( const LEDImageBase<bool>& glyph, int row, int column ) {
	if ( row >= this->rows()
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

	for ( int yT = startRow, yO = imageY; (yT < this->rows()) && (yO < glyph.rows()); yT++, yO++ ) { 
		for (int colCounter = 0; colCounter < imageColumns; colCounter++) {
			if (glyph.pixel(yO, imageX+colCounter)) {
				this->setBit(yT, thisX+colCounter);
			} else {
				this->clearBit(yT, thisX+colCounter);
			}
		}
	}
}


#if 0

MutableRGBImage* GlyphBase::getImageWithColor( RGBColorType foreground, RGBColorType background ) const {
	MutableRGBImage* img = new MutableRGBImage( this->rows(), this->columns() );
	
	if (this->isProgMem()) {
		// creating a PROGMEM version of the loops for speed: don't have to check if PROGRMEM
		// with every loop in getBits()
		for (int y = 0; y < this->rows(); y++) {
			for (int x = 0; x < this->columns(); x++) {
				int bitIdx = y*this->columns() + x;
				img->pixel(y, x) = (bool)pgm_read_byte_near( this->bits() + bitIdx ) ? foreground : background;
			}
		}	
	} else {
		for (int y = 0; y < this->rows(); y++) {
			for (int x = 0; x < this->columns(); x++) {
				img->pixel(y, x) = this->getBit(y,x) ? foreground : background;
			}
		}
	}
	return img;
}
#endif
