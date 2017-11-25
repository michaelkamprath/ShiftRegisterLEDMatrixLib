//     Shift Register LED Matrix Project - RGBLEDMatrix
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
  

	/**
	 *	Constructs a mono-color LED matrix controller object.
	 *
	 *	@param rows the number of  rows in the LED matrix. Rows are expected to have
	 *	            shared power to the LEDs.
	 *	@param columns the number of columns in the LED matrix.
	 *	@param columnControlBitOn what value a column bit should be set to to the column on.
	 *	@param rowControlBitOn what value a row bit should be set to to turn the row on. 
	 *               E.g. of the row is common anode and a PNP transistor is being use 
	 *               to switch power to the row, the row bit likely needs to be LOW to
	 *               cause the transistor to power the row.
	 *  @param slavePin which ard pin is used for the latch signal.
	 */
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
	
	/**
	 * Should be called before any operations against this object is performed.
	 */
	virtual void setup();
	
	/**
	 * Returns the image buffer for this matrix object. This is the 
	 * image buffer that drawing should be done to.
	 *
	 * @return a MutableGlyph object reference that is the matrix's image buffer.
	 */ 
	MutableGlyph& image(void)				{ return *_screen_data; }

	/**
	 * Returns a const reference to the image buffer for this matrix object. This is the 
	 * image buffer that drawing should be done to, but is const here and thus should
	 * be only read from.
	 *
	 * @return a const MutableGlyph object reference that is the matrix's image buffer.
	 */ 
	const MutableGlyph& image(void) const	{ return *_screen_data; }
};

#endif //__SCREEN_H__
