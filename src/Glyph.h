//     RGB Matrix Project - Glyph
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

#ifndef __GLYPH_H__
#define __GLYPH_H__

#include "RGBImage.h"

/***
 * Glyph
 *
 * A Glyph is N x M array of bits that indicates pixels in an image devoid of color.
 * Glyphs are generally used to represent the image of letters in a font, but can be
 * used for any pixel image.
 *
 */

class GlyphBase {
private:
	
	int _rows;
	int _columns;

public:
	static bool* generateBitBoolArray( 
			int rows,
			int columns,
			const unsigned char* data,
			bool isFromProgramSpace
		);

	GlyphBase( 
			int rows,
			int columns
		);
	GlyphBase( const GlyphBase& GlyphBase );
	virtual ~GlyphBase();
	virtual const bool* bits(void) const = 0;
	
	int rows(void) const		{ return _rows; }
	int columns(void) const		{ return _columns; }
	virtual bool isProgMem(void) const = 0;
	
	virtual bool getBit( int row, int column ) const;
	
	MutableRGBImage* getImageWithColor(
			ColorType foreground,
			ColorType background = TRANSPARENT_COLOR
		) const;

	void streamToSerial(void);
};

class MutableGlyph : public GlyphBase {
private:
	bool* _bits;
	bool _dirty;
		
public:

	MutableGlyph( 
			int rows,
			int columns,
			const unsigned char* data = NULL,
			bool isFromProgramSpace = false
		);
	MutableGlyph( const GlyphBase& other );
	virtual ~MutableGlyph();
	virtual const bool* bits(void) const	{ return _bits; }
	bool* bits(void)						{ return _bits; }
	
	virtual bool isProgMem(void) const		{ return false; }
	
	void eraseAll();
	void setBit( int row, int column );
	void clearBit( int row, int column );

	bool isDirty( void) const				{ return _dirty; }
	void setNotDirty()						{ _dirty = false; }
	
	void placeGlyphAt( const GlyphBase& glyph, int row, int column );
	void drawLine(
		int startRow,
		int startColumn,
		int stopRow,
		int stopColumn
	);
	void drawRectangle( 
		int tlRow,
		int tlColumn,
		int brRow,
		int brColumn,
		bool fill = true
	);


};
 
class Glyph : public GlyphBase {
private:
	const bool* _bits;
	bool _isProgMem;
	bool _manageMem;
			
public:

	Glyph( 
			int rows,
			int columns,
			const unsigned char* data,
			bool isFromProgramSpace = false
		);
	Glyph( 
			int rows,
			int columns,
			const bool* data,
			bool isFromProgramSpace = false
		);
	Glyph( const GlyphBase& other );
	virtual ~Glyph();	
	virtual const bool* bits(void) const		{ return _bits; };
	
	virtual bool isProgMem(void) const			{ return _isProgMem; }

};


#endif
