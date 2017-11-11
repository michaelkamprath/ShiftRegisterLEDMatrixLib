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
		// are order Red, Green Blue
		// For a 4-column matrix, each row is ordered: RRRR-GGGG-BBBB
		RGB_GROUPS
	} RGBLEDBitLayout;

private:
	const RGBLEDBitLayout _bitLayout;
	MutableRGBImage *_screen_data;
	

	void setRowBitsForFrame(
			int row,
			size_t frame,
			LEDMatrixBits& frameBits,
			const RGBImageBase& image
		) const; 
	size_t maxFrameCountForValue(ColorType value) const;
protected:
	virtual void generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const;
	virtual bool matrixNeedsUpdate(void) const;
	virtual void matrixHasBeenUpdated(void);
	virtual unsigned int multiplier5microseconds( size_t frame ) const;
public:
  

	RGBLEDMatrix(
			int rows,
			int columns,
			RGBLEDBitLayout bitLayout = INDIVIDUAL_LEDS,
			bool columnControlBitOn = LOW,
			bool rowControlBitOn = LOW,
#if defined ( ESP8266 )
			int slavePin = D8	
#else
			int slavePin = 10	
#endif
		);
	virtual ~RGBLEDMatrix();
	
	virtual void setup();
	
	MutableRGBImage& image(void)				{ return *_screen_data; }
	const MutableRGBImage& image(void) const	{ return *_screen_data; }
  
};

#endif //__SCREEN_H__
