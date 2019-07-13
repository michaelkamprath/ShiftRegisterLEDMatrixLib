//     Shift Register LED Matrix Project - LEDMatrix
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
#include <Adafruit_GFX.h>
#include "BaseLEDMatrix.h"

typedef uint16_t LEDColor;

const LEDColor LED_BLACK = 0;
const LEDColor LED_WHITE = 1;

class LEDMatrix : public BaseLEDMatrix, public GFXcanvas1 {
	
private:
	bool		_matrixNeedsUpdate;
	
	void setRowBitsForFrame(
			uint16_t row,
			size_t frame,
			LEDMatrixBits& frameBits
		) const;

protected:
	virtual void generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const;
	virtual bool matrixNeedsUpdate(void) const;
	virtual void matrixHasBeenUpdated(void);
	virtual unsigned int baseIntervalMultiplier( size_t frame ) const;

	uint16_t rawPixel( int16_t rawX, int16_t rawY ) const;  
	
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
	 *	@param interFrameOffTimeMicros Controls whether an all-off signal is sent to the
	 *				 shift registers in between row updates in order to mitigate LED 
	 *				 ghosting. A value of 0 indicates that no all-off signal should be 
	 *				 sent. A value greater than 0 indicates how many microseconds should
	 *				 be waited until the next row update gets shifted out. Usually a value 
	 *				 less than 3 microseconds is sufficient for most slow row power
	 *				 switching.
	 *  @param slavePin which ard pin is used for the latch signal.
	 */
	LEDMatrix(
			uint16_t rows,
			uint16_t columns,
			bool columnControlBitOn = LOW,
			bool rowControlBitOn = LOW,
			unsigned int interFrameOffTimeMicros = 0,
#if defined ( ESP8266 )
			int slavePin = D8	
#elif defined( ESP32 )
			int slavePin = 5	
#else
			int slavePin = 10	
#endif
		);
	virtual ~LEDMatrix();
	
	/**
	 * Should be called before any operations against this object is performed.
	 */
	virtual void setup();

	//
	// Adafruit GFX Support
	//
	
	uint16_t pixel( int16_t x, int16_t y ) const;  
	virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
	virtual void fillScreen(uint16_t color);
};

#endif //__SCREEN_H__
