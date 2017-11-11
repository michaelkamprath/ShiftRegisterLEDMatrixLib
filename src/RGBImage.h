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
#ifndef __RGBIMAGE12BIT_H__
#define __RGBIMAGE12BIT_H__

#ifndef TWENTY_FOUR_BIT_COLOR
#define TWENTY_FOUR_BIT_COLOR (defined(__arm__) && defined(TEENSYDUINO))||defined(__AVR_ATmega2560__)||defined ( ESP8266 )
#endif
/***

Each byte controls one LED. Each color get four bits to indicate color intensity (0-15).
The bits of the byte are laid out as follows in a 2-byte integer:

	Bits   0   4   8  12  
		   |---|---|---|---
 		   TUUURRRRGGGGBBBB

	T = transparent
	U = unused
	R = Red
	G = Green
	B = Blue

*/
class GlyphBase;

#if TWENTY_FOUR_BIT_COLOR
typedef unsigned int ColorType;


const ColorType AQUA_COLOR = 0x00FF;
const ColorType BLACK_COLOR = 0;
const ColorType BLUE_COLOR = 0x000F;
const ColorType	BROWN_COLOR = 0x0941;
const ColorType CORAL_COLOR = 0x0F75;
const ColorType DARK_BLUE_COLOR = 0x0004;
const ColorType DARK_GRAY_COLOR = 0x0444;
const ColorType DARK_GREEN_COLOR = 0x0040;
const ColorType DARK_RED_COLOR = 0x0400;
const ColorType GRAY_COLOR = 0x0999;
const ColorType GREEN_COLOR = 0x00F0;
const ColorType LIME_COLOR = 0x03C3;
const ColorType MAGENTA_COLOR = 0x0F0F;
const ColorType ORANGE_COLOR = 0x0FA0;
const ColorType PINK_COLOR = 0x0F6A;
const ColorType PURPLE_COLOR = 0x092E;
const ColorType RED_COLOR = 0x0F00;
const ColorType SKY_BLUE_COLOR = 0x08CF;
const ColorType SLATE_BLUE_COLOR = 0x065C;
const ColorType TRANSPARENT_COLOR = 0x8000;
const ColorType TURQUOISE_COLOR = 0x05FA;
const ColorType VIOLET_COLOR = 0x0E7E;
const ColorType WHITE_COLOR = 0x0FFF;
const ColorType YELLOW_COLOR = 0x0FF0;

const static ColorType RED_MASK = 0x0F00;
const static unsigned int RED_BIT_SHIFT = 8;

const static ColorType GREEN_MASK = 0x00F0;
const static unsigned int GREEN_BIT_SHIFT = 4;

const static ColorType BLUE_MASK = 0x000F;
const static unsigned int BLUE_BIT_SHIFT = 0;

#else

/***
Each byte controls one LED. Each color get two bits to indicate color intensity (0,1,2,3).
The bits of the byte are laid out as follows:
	Bits   0   4
		   |---|---
		   TURRGGBB
	T = transparent
	U = unused
	R = Red
	G = Green
	B = Blue
*/

typedef unsigned char ColorType;

const ColorType AQUA_COLOR = 0x0F;
const ColorType BLACK_COLOR = 0;
const ColorType BLUE_COLOR = 0x03;
const ColorType CORAL_COLOR = 0x39;
const ColorType DARK_BLUE_COLOR = B00000001;
const ColorType DARK_GRAY_COLOR = 0x15;
const ColorType DARK_GREEN_COLOR = B00000100;
const ColorType DARK_RED_COLOR = B00010000;
const ColorType GRAY_COLOR = 0x2A;
const ColorType GREEN_COLOR = 0x0C;
const ColorType LIME_COLOR = 0x2D;
const ColorType MAGENTA_COLOR = 0x33;
const ColorType ORANGE_COLOR = 0x38;
const ColorType PINK_COLOR = 0x26;
const ColorType PURPLE_COLOR = B00010001;
const ColorType RED_COLOR = 0x30;
const ColorType SKY_BLUE_COLOR = B00010110;
const ColorType SLATE_BLUE_COLOR = 0x16;
const ColorType TRANSPARENT_COLOR = B10000000;
const ColorType TURQUOISE_COLOR = 0x1E;
const ColorType VIOLET_COLOR = 0x27;
const ColorType WHITE_COLOR = 0x3F;
const ColorType YELLOW_COLOR = 0x3C;

const static ColorType RED_MASK = 0x30;
const static unsigned char RED_BIT_SHIFT = 4;

const static ColorType GREEN_MASK = 0x0C;
const static unsigned char GREEN_BIT_SHIFT = 2;

const static ColorType BLUE_MASK = 0x03;
const static unsigned char BLUE_BIT_SHIFT = 0;
#endif

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
	virtual const ColorType* data(void) const = 0;

	const ColorType pixel( int row, int column ) const;
	
};

class RGBImage : public RGBImageBase {
private:

	const ColorType* _data;
	int _dataSize;
	bool _isProgMem;
	bool _manageMem;

public:

	RGBImage(
			int rows,
			int columns,
			const ColorType* data,
			bool isFromProgramSpace = false
		);
	RGBImage(const RGBImageBase& other);
	virtual ~RGBImage();

	virtual bool isProgMem(void) const			{ return _isProgMem; }
	virtual const ColorType* data(void) const	{ return _data; }

};

class MutableRGBImage : public RGBImageBase {
private:
	ColorType* _data;
	
	bool _dirty;

public:
	MutableRGBImage(int rows, int columns);
	MutableRGBImage(
			int rows,
			int columns,
			const ColorType* data,
			bool isFromProgramSpace = false
		);
	MutableRGBImage(const RGBImageBase& other);
	virtual ~MutableRGBImage();

	virtual bool isProgMem(void) const		{ return false; }
	virtual const ColorType* data(void) const	{ return _data; }

	bool isDirty( void) const	{ return _dirty; }
	void setNotDirty()			{ _dirty = false; }
	
	void copy(const RGBImageBase& other);

	ColorType& pixel( int row, int column );
		
	void placeImageAt( const RGBImageBase& image, int row, int column );
	void paintColor( ColorType color ); 
	void drawLine(
		int startRow,
		int startColumn,
		int stopRow,
		int stopColumn,
		ColorType color
	);
	void drawRectangle( 
		int tlRow,
		int tlColumn,
		int brRow,
		int brColumn,
		ColorType color,
		bool fill = true
	);
	
	void drawGlyph(
			const GlyphBase& image,
			int row,
			int column,
			ColorType foreground,
			ColorType background = TRANSPARENT_COLOR
		);

};

#endif //__RGBIMAGE_H__

