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
#ifndef __RGBIMAGE12BIT_H__
#define __RGBIMAGE12BIT_H__
#include "RGBColor.h"

class GlyphBase;

class RGBImageBase {
private:
	int _rows;
	int _columns;

public:
	RGBImageBase(int rows, int columns);
	RGBImageBase(const RGBImageBase& other);
	virtual ~RGBImageBase();
	
	int rows(void) const		{ return _rows; }
	int columns(void) const		{ return _columns; }
	virtual bool isProgMem(void) const = 0;
	virtual const RGBColorType* data(void) const = 0;

	const RGBColorType pixel( int row, int column ) const;
	
};

class RGBImage : public RGBImageBase {
private:

	const RGBColorType* _data;
	int _dataSize;
	bool _isProgMem;
	bool _manageMem;

public:

	RGBImage(
			int rows,
			int columns,
			const RGBColorType* data,
			bool isFromProgramSpace = false
		);
	RGBImage(const RGBImageBase& other);
	virtual ~RGBImage();

	virtual bool isProgMem(void) const			{ return _isProgMem; }
	virtual const RGBColorType* data(void) const	{ return _data; }

};

class MutableRGBImage : public RGBImageBase {
private:
	RGBColorType* _data;
	
	bool _dirty;

public:
	MutableRGBImage(int rows, int columns);
	MutableRGBImage(
			int rows,
			int columns,
			const RGBColorType* data,
			bool isFromProgramSpace = false
		);
	MutableRGBImage(const RGBImageBase& other);
	virtual ~MutableRGBImage();

	virtual bool isProgMem(void) const		{ return false; }
	virtual const RGBColorType* data(void) const	{ return _data; }

	bool isDirty( void) const	{ return _dirty; }
	void setNotDirty()			{ _dirty = false; }
	
	void copy(const RGBImageBase& other);

	RGBColorType& pixel( int row, int column );
		
	void placeImageAt( const RGBImageBase& image, int row, int column );
	void paintColor( RGBColorType color ); 
	void drawLine(
		int startRow,
		int startColumn,
		int stopRow,
		int stopColumn,
		RGBColorType color
	);
	void drawRectangle( 
		int tlRow,
		int tlColumn,
		int brRow,
		int brColumn,
		RGBColorType color,
		bool fill = true
	);
	
	void drawGlyph(
			const GlyphBase& image,
			int row,
			int column,
			RGBColorType foreground,
			RGBColorType background = TRANSPARENT_COLOR
		);

};

#endif //__RGBIMAGE_H__

