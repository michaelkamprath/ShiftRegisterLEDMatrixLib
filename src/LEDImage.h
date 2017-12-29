//     Shift Register LED Matrix Project - LEDImage
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

#ifndef __LEDIMAGE_H__
#define __LEDIMAGE_H__

/*
 * LEDImage
 *
 * This is a templated class for managing an image composed of a generic 
 * PixelType. This allows the single implementation of low-level logic for image
 * management, and then instantiations of this template indicate the exact type
 * the pixel will be. PixelType should be a primitive.
 *
 */

#include <Arduino.h>
#include <string.h>
#if defined ( ESP8266 )
  #include <pgmspace.h>
#else
  #include <avr/pgmspace.h>
#endif

#pragma mark LEDImageBase

template <class PixelType> 
class LEDImageBase {
private:
	int _rows;
	int _columns;

public:
	LEDImageBase(int rows, int columns);
	LEDImageBase(const LEDImageBase<PixelType>& other);
	virtual ~LEDImageBase();
	
	int rows(void) const		{ return _rows; }
	int columns(void) const		{ return _columns; }
	virtual bool isProgMem(void) const = 0;
	virtual const PixelType* data(void) const = 0;

	const PixelType pixel( int row, int column ) const;
	
};



template <class PixelType>
LEDImageBase<PixelType>::LEDImageBase(int rows, int columns)
:	_rows(rows),
	_columns(columns)
{
}

template <class PixelType>
LEDImageBase<PixelType>::LEDImageBase(const LEDImageBase<PixelType>& other)
:	_rows(other.rows()),
	_columns(other.columns())
{
}

template <class PixelType>
LEDImageBase<PixelType>::~LEDImageBase()
{
}

template <class PixelType>
const PixelType LEDImageBase<PixelType>::pixel( int row, int column ) const {
	if (this->isProgMem()) {
		switch (sizeof(PixelType)) {
			case 4:
				return (PixelType)pgm_read_word_near( this->data() + row*this->columns() + column );
				break;
			default:
			case 2:
				return (PixelType)pgm_read_byte_near( this->data() + row*this->columns() + column );
				break;
		}
	} else {
		return this->data()[row*this->columns() + column];
	}
}

#pragma mark LEDImage

template <class PixelType> 
class LEDImage : public LEDImageBase<PixelType> {
private:

	const PixelType* _data;
	int _dataSize;
	bool _isProgMem;
	bool _manageMem;

public:

	LEDImage(
			int rows,
			int columns,
			const PixelType* data,
			bool isFromProgramSpace = false
		);
	LEDImage(const LEDImageBase<PixelType>& other);
	virtual ~LEDImage();

	virtual bool isProgMem(void) const			{ return _isProgMem; }
	virtual const PixelType* data(void) const	{ return _data; }

};

template <class PixelType>
LEDImage<PixelType>::LEDImage(
	int rows,
	int columns,
	const PixelType* data,
	bool isFromProgramSpace )
:	LEDImageBase<PixelType>(rows,columns),
	_data(data),
	_isProgMem(isFromProgramSpace),
	_manageMem(false)
{
}

template <class PixelType>
LEDImage<PixelType>::LEDImage(const LEDImageBase<PixelType>& other)
:	LEDImageBase<PixelType>(other),
	_data( other.isProgMem() ? 
				other.data() :
				(PixelType*)memcpy(
					new PixelType[(other.rows()*other.columns())],
					other.data(),
					(other.rows()*other.columns())*sizeof(PixelType)
				)),
	_isProgMem(false),
	_manageMem(true)
{

}

template <class PixelType>
LEDImage<PixelType>::~LEDImage() {
	if (_manageMem) {
		delete _data;
	}
}


#pragma mark MutableLEDImage

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
class MutableLEDImage : public LEDImageBase<PixelType> {
private:
	PixelType* _data;
	
	bool _dirty;

protected:

	void makeDirty(void) 		{ _dirty = true; }
	
public:
	MutableLEDImage(int rows, int columns);
	MutableLEDImage(
			int rows,
			int columns,
			const PixelType* data,
			bool isFromProgramSpace = false
		);
	MutableLEDImage(const LEDImageBase<PixelType>& other);
	virtual ~MutableLEDImage();

	virtual bool isProgMem(void) const			{ return false; }
	virtual const PixelType* data(void) const	{ return _data; }

	bool isDirty(void) const	{ return _dirty; }
	void setNotDirty()			{ _dirty = false; }
	
	void copy(const LEDImageBase<PixelType>& other);

	using LEDImageBase<PixelType>::pixel;
	PixelType& pixel( int row, int column );
		
	void placeImageAt( const LEDImageBase<PixelType>& image, int row, int column );
	void paintColor( PixelType color ); 
	void drawLine(
		int startRow,
		int startColumn,
		int stopRow,
		int stopColumn,
		PixelType color
	);
	void drawRectangle( 
		int tlRow,
		int tlColumn,
		int brRow,
		int brColumn,
		PixelType color,
		bool fill = true
	);
	
};

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::MutableLEDImage(int rows, int columns)
:	LEDImageBase<PixelType>(rows,columns),
	_data(new PixelType[rows*columns]),
	_dirty(false)
{
	this->paintColor(LEDBlackColor);
}

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::MutableLEDImage(
	int rows,
	int columns,
	const PixelType* data,
	bool isFromProgramSpace
) :	LEDImageBase<PixelType>(rows,columns),
	_data(new PixelType[rows*columns]),
	_dirty(false)
{
	if (isFromProgramSpace) {
		memcpy_P(_data, data, rows*columns*sizeof(PixelType));		
	} else {
		memcpy(_data, data, rows*columns*sizeof(PixelType));
	}
}

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::MutableLEDImage(
	const LEDImageBase<PixelType>& other
) :	LEDImageBase<PixelType>(other),
	_data(new PixelType[other.rows()*other.columns()]),
	_dirty(true)
{
	this->copy(other);
}

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::~MutableLEDImage() {
	delete _data;
}

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
void MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::copy(
	const LEDImageBase<PixelType>& other
) {
	// only copy if same dimensions
	
	if (other.rows() == this->rows() && other.columns() == this->columns()) {
		if (other.isProgMem()) {
			memcpy_P(_data, other.data(), other.rows()*other.columns()*sizeof(PixelType));		
		} else {
			memcpy(_data, other.data(), other.rows()*other.columns()*sizeof(PixelType));
		}
		_dirty = true;
	}
}

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
PixelType& MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::pixel(
	int row,
	int column
) { 
	_dirty = true;
	return _data[row*this->columns() + column];
}

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
void MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::placeImageAt(
		const LEDImageBase<PixelType>& image,
		int row,
		int column
	)
{
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
			PixelType color = image.pixel(yO,imageX+colCounter);
 			if (color != LEDTransparentColor) {
				this->pixel(yT,thisX+colCounter) = color;
 			}
		}
	}
}

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
void MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::paintColor( PixelType color ) {
	_dirty = true;
	for (int idx = 0; idx < this->rows()*this->columns(); idx++) {
		_data[idx] = color;
	}
}

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
void MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::drawLine( 
		int startRow,
		int startColumn,
		int stopRow,
		int stopColumn,
		PixelType color
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

template <class PixelType, PixelType LEDBlackColor, PixelType LEDTransparentColor> 
void MutableLEDImage<PixelType,LEDBlackColor,LEDTransparentColor>::drawRectangle( 
  		int tlRow,
  		int tlColumn,
  		int brRow,
  		int brColumn,
  		PixelType color,
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
			PixelType* startPtr = &this->pixel(row,finalTLColumn);
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

#endif //__LEDIMAGE_H__
