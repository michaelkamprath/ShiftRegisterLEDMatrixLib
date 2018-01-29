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

#ifndef __RGBLEDMATRIX_H__
#define __RGBLEDMATRIX_H__
#include "BaseLEDMatrix.h"
#include "RGBImage.h"

class RGBLEDMatrix : public BaseLEDMatrix {
public:
	typedef enum {
		// Each individual LED's RGB bits are consecutive in column order.
		// For a 4-column matrix, each row is ordered: RGB-RGB-RGB-RGB
		INDIVIDUAL_LEDS,	
		
		// All elements of the same color are consecutive in column order. The color
		// are order Red, Green, Blue
		// For a 4-column matrix, each row is ordered: RRRR-GGGG-BBBB
		RGB_GROUPS,
		
		// All elements of the same color are consecutive in column order. The color
		// are order Red, Blue, Green
		// For a 4-column matrix, each row is ordered: RRRR-BBBB-GGGG
		RBG_GROUPS
			
	} RGBLEDBitLayout;

private:
	const RGBLEDBitLayout _bitLayout;
	MutableRGBImage *_screen_data;
	

	void setRowBitsForFrame(
			int row,
			size_t frame,
			LEDMatrixBits& frameBits,
			const MutableRGBImage& image
		) const; 
	size_t maxFrameCountForValue(RGBColorType value) const;
protected:
	virtual void generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const;
	virtual bool matrixNeedsUpdate(void) const;
	virtual void matrixHasBeenUpdated(void);
	virtual unsigned int baseIntervalMultiplier( size_t frame ) const;
public:
  
	/**
	 *	Constructs an RGB LED matrix controller object.
	 *
	 *	@param rows the number of  rows in the LED matrix. Rows are expected to have
	 *	            shared power to the LEDs.
	 *	@param columns the number of columns in the LED matrix.
	 *	@param bitLayout an RGBLEDBitLayout enum indicating how the RGB bits are arranged 
	 *              in each row.
	 *	@param columnControlBitOn what value a column bit should be set to to the column on.
	 *	@param rowControlBitOn what value a row bit should be set to to turn the row on. 
	 *               E.g. of the row is common anode and a PNP transistor is being use 
	 *               to switch power to the row, the row bit likely needs to be LOW to
	 *               cause the transistor to power the row.
	 *	@param interFrameOffTimeMicros Controls whether an all-off signal is sent to the
	 *				 shift registers in between row updates in order to mitigate LED 
	 *				 ghosting. A value of 0 indicates that no all-off signal should be 
	 *				 sent. A value greater than 0 indicates how many microseconds should
	 *				 be waited until the next row update gets shifted out. Usually a value 
	 *				 less than 3 microseconds is sufficient for most slow row power
	 *				 switching.
	 *  @param slavePin which ard pin is used for the latch signal.
	 */
	RGBLEDMatrix(
			int rows,
			int columns,
			RGBLEDBitLayout bitLayout = INDIVIDUAL_LEDS,
			bool columnControlBitOn = LOW,
			bool rowControlBitOn = LOW,
			unsigned int interFrameOffTimeMicros = 0,
#if defined ( ESP8266 )
			int slavePin = D8	
#else
			int slavePin = 10	
#endif
		);
	virtual ~RGBLEDMatrix();
	
	virtual void setup();
	
	/**
	 * Returns the image buffer for this matrix object. This is the 
	 * image buffer that drawing should be done to.
	 *
	 * @return a MutableRGBImage object reference that is the matrix's image buffer.
	 */ 
	MutableRGBImage& image(void)				{ return *_screen_data; }

	/**
	 * Returns a const reference to the image buffer for this matrix object. This is the 
	 * image buffer that drawing should be done to, but is const here and thus should
	 * be only read from.
	 *
	 * @return a const MutableRGBImage object reference that is the matrix's image buffer.
	 */ 
	const MutableRGBImage& image(void) const	{ return *_screen_data; }
  
};

#endif //__SCREEN_H__
