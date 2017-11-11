//     RGB Matrix Project - RGBImage
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
#include <string.h>
#if defined ( ESP8266 )
  #include <pgmspace.h>
#else
  #include <avr/pgmspace.h>
#endif
#include "RGBImage.h"
#include "Glyph.h"

#pragma mark RGBImageBase

RGBImageBase::RGBImageBase(int rows, int columns)
:	_rows(rows),
	_columns(columns)
{
}

RGBImageBase::RGBImageBase(const RGBImageBase& other)
:	_rows(other.rows()),
	_columns(other.columns())
{
}

RGBImageBase::~RGBImageBase()
{
}

const ColorType RGBImageBase::pixel( int row, int column ) const {
	if (this->isProgMem()) {
#if TWENTY_FOUR_BIT_COLOR
		return (ColorType)pgm_read_word_near( this->data() + row*this->columns() + column );
#else
		return (ColorType)pgm_read_byte_near( this->data() + row*this->columns() + column );
#endif
	} else {
		return this->data()[row*this->columns() + column];
	}
}

#pragma mark RGBImage

RGBImage::RGBImage(
	int rows,
	int columns,
	const ColorType* data,
	bool isFromProgramSpace )
:	RGBImageBase(rows,columns),
	_data(data),
	_isProgMem(isFromProgramSpace),
	_manageMem(false)
{
}

RGBImage::RGBImage(const RGBImageBase& other)
:	RGBImageBase(other),
	_data( other.isProgMem() ? 
				other.data() :
				(ColorType*)memcpy(
					new ColorType[(other.rows()*other.columns())],
					other.data(),
					(other.rows()*other.columns())*sizeof(ColorType)
				)),
	_isProgMem(false),
	_manageMem(true)
{

}

RGBImage::~RGBImage() {
	if (_manageMem) {
		delete _data;
	}
}


#pragma mark MutableRGBImage


MutableRGBImage::MutableRGBImage(int rows, int columns)
:	RGBImageBase(rows,columns),
	_data(new ColorType[rows*columns]),
	_dirty(false)
{
	this->paintColor(BLACK_COLOR);
}

MutableRGBImage::MutableRGBImage(
	int rows,
	int columns,
	const ColorType* data,
	bool isFromProgramSpace )
:	RGBImageBase(rows,columns),
	_data(new ColorType[rows*columns]),
	_dirty(false)
{
	if (isFromProgramSpace) {
		memcpy_P(_data, data, rows*columns*sizeof(ColorType));		
	} else {
		memcpy(_data, data, rows*columns*sizeof(ColorType));
	}
}

MutableRGBImage::MutableRGBImage(const RGBImageBase& other)
:	RGBImageBase(other),
	_data(new ColorType[other.rows()*other.columns()]),
	_dirty(true)
{
	this->copy(other);
}

MutableRGBImage::~MutableRGBImage() {
	delete _data;
}

void MutableRGBImage::copy(const RGBImageBase& other) {
	// only copy if same dimensions
	
	if (other.rows() == this->rows() && other.columns() == this->columns()) {
		if (other.isProgMem()) {
			memcpy_P(_data, other.data(), other.rows()*other.columns()*sizeof(ColorType));		
		} else {
			memcpy(_data, other.data(), other.rows()*other.columns()*sizeof(ColorType));
		}
		_dirty = true;
	}
}

ColorType& MutableRGBImage::pixel( int row, int column ) { 
	_dirty = true;
	return _data[row*this->columns() + column];
}

void MutableRGBImage::placeImageAt( const RGBImageBase& image, int row, int column ) {
	if (	row >= this->rows()
			|| column >= this->columns()
			|| (image.columns() + column <= 0 )
			|| (image.rows() + row <= 0) 
		) {
		return;
	}

	_dirty = true;
	int imageX = 0;
	int thisX = column;
	if (column < 0) {
		imageX = -column;
		thisX = 0;
	}
	
	int imageColumns = image.columns() - imageX;
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

	for ( int yT = startRow, yO = imageY; (yT < this->rows()) && (yO < image.rows()); yT++, yO++ ) { 
		for (int colCounter = 0; colCounter < imageColumns; colCounter++) {
			ColorType color = image.pixel(yO,imageX+colCounter);
			if (color != TRANSPARENT_COLOR) {
				this->pixel(yT,thisX+colCounter) = color;
			}
		}
	}
}

void MutableRGBImage::paintColor( ColorType color ) {
	_dirty = true;
	for (int idx = 0; idx < this->rows()*this->columns(); idx++) {
		_data[idx] = color;
	}
}

void MutableRGBImage::drawLine( 
		int startRow,
		int startColumn,
		int stopRow,
		int stopColumn,
		ColorType color
	)
{
	_dirty = true;
	if ( stopColumn != startColumn ) {
		float delta_col = stopColumn - startColumn;
		float delta_row = stopRow - startRow;
		
		if (abs(delta_col) > abs(delta_row)) {
			for (int col = startColumn; 
				delta_col < 0 ? col >= stopColumn : col <= stopColumn; 
				delta_col < 0 ? col-- : col++ ) 
			{
				int row = round(startRow + delta_row*(col - startColumn)/delta_col);
				this->pixel(row,col) = color;
			}
		}
		else {
			for (int row = startRow; 
				delta_row < 0 ? row >= stopRow : row <= stopRow; 
				delta_row < 0 ? row-- : row++ ) 
			{
				int col = round(startColumn + delta_col*(row - startRow)/delta_row);
				this->pixel(row,col) = color;
			}
		
		}
	}
	else {
		float delta_row = stopRow - startRow;

		for ( int row = startRow; 
			  delta_row < 0 ? row >= stopRow : row <= stopRow; 
			  delta_row < 0 ? row-- : row++ ) 
		{
			this->pixel(row,startColumn) = color;
		}	
	}
}	

void MutableRGBImage::drawRectangle( 
  		int tlRow,
  		int tlColumn,
  		int brRow,
  		int brColumn,
  		ColorType color,
  		bool fill
  	)
{
	if ( tlRow > brRow || tlColumn > brColumn 
		|| tlRow >= this->rows() || brRow < 0 
		|| tlColumn >= this->columns() || brColumn < 0) {
		return;
	}
	
	_dirty = true;
	int finalTLRow = min(max(0,tlRow),this->rows()-1);
	int finalBRRow = min(max(0,brRow),this->rows()-1);
	int finalTLColumn = min(max(0,tlColumn),this->columns()-1);
	int finalBRColumn = min(max(0,brColumn),this->columns()-1);
	
	for (int row = finalTLRow; row <= finalBRRow; row++ ) {
		if ( fill || row == finalTLRow || row == finalBRRow ) {
			ColorType* startPtr = &this->pixel(row,finalTLColumn);
			for (int i = 0; i < finalBRColumn-finalTLColumn+1; i++) {
				startPtr[i] = color;
			}
		}
		else {
			this->pixel(row,finalTLColumn) = color;
			this->pixel(row,finalBRColumn) = color;
		}
	}
}

void MutableRGBImage::drawGlyph(
		const GlyphBase& glyph,
		int row,
		int column,
		ColorType foreground,
		ColorType background
	)
{
	if (	row >= this->rows()
			|| column >= this->columns()
			|| (glyph.columns() + column <= 0 )
			|| (glyph.rows() + row <= 0) 
		) {
		return;
	}

	_dirty = true;
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
			if (glyph.getBit(yGlyph,imageX+colCounter)) {
				this->pixel(yT,thisX+colCounter) = foreground;
			}
			else if ( background != TRANSPARENT_COLOR ) {
				this->pixel(yT,thisX+colCounter) = background;
			}
		}
	}
}
