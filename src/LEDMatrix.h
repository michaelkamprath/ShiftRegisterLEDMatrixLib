//     RGB Matrix Project - RGBLEDMatrix
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

#ifndef __LEDMATRIX_H__
#define __LEDMATRIX_H__
#include "BaseLEDMatrix.h"
#include "Glyph.h"


class LEDMatrix : public BaseLEDMatrix {
	
private:
	MutableGlyph *_screen_data;	
	
	void setRowBitsForFrame(
			int row,
			size_t frame,
			LEDMatrixBits& frameBits,
			const GlyphBase& image
		) const;

protected:
	virtual void generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const;
	virtual bool matrixNeedsUpdate(void) const;
	virtual void matrixHasBeenUpdated(void);
	
public:
  

	LEDMatrix(
			int rows,
			int columns,
			bool columnControlBitOn = LOW,
			bool rowControlBitOn = LOW,
#if defined ( ESP8266 )
			int slavePin = D8	
#else
			int slavePin = 10	
#endif
		);
	virtual ~LEDMatrix();
	
	virtual void setup();
	
	MutableGlyph& image(void)				{ return *_screen_data; }
	const MutableGlyph& image(void) const	{ return *_screen_data; }
};

#endif //__SCREEN_H__
