//     Shift Register LED Matrix Project - BaseLEDMatrix
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

#ifndef __BASELEDMATRIX_H__
#define __BASELEDMATRIX_H__
#include <Arduino.h>
#include "TimerAction.h"
#include "LEDMatrixBits.h"
#include "SPIConnection.h"

#define LED_MATRIX_MAX_SCAN_PASS_COUNT 1

#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif

class BaseLEDMatrix : public TimerAction {
	
private:
	unsigned int _rows;
	unsigned int _columns;
	unsigned int _columnBitWidth;
	unsigned int _pwmCycleScanCount;
	unsigned int _interFrameOffTimeMicros;
	unsigned int _interFrameOffTimeInterval;
	 
	bool _columnControlBitOn;
	bool _rowControlBitOn;
		
	LEDMatrixBits **_curScreenBitFrames;
	LEDMatrixBits **_screenBitFrames;
	LEDMatrixBits *_allOffBits;
	
	bool _screenBitFrameToggle;
	bool _interFrameTransmitOffToggle;
	
	unsigned int _scanPass;
	unsigned int _scanRow;
	int _isDrawingCount;

	SPIConnection	_spi;

	void shiftOutRow( int row, int scanPass );
	
protected:
	virtual void action();
	
	virtual void generateFrameBits(LEDMatrixBits& frameBits, size_t frame ) const = 0;
	virtual bool matrixNeedsUpdate(void) const = 0;
	virtual void matrixHasBeenUpdated(void) = 0;
	
	virtual unsigned int baseIntervalMultiplier( size_t frame ) const;
public:

	/**
	 *	Constructs the base LED matrix controller object.
	 *
	 *	@param rows the number of  rows in the LED matrix. Rows are expected to have
	 *	            shared power to the LEDs.
	 *	@param columns the number of columns in the LED matrix.
	 *	@param columnBitWidth the number of bits needed to represent one physical LED in 
	 *              a column. E.g., and RGB LED needs 3 bits.
	 *	@param pwmCycleScanCount the number of row scans needed for the PWM cycle of the matrix.
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
	 *  @param slavePin which pin is used for the latch signal.
	 *  @param maxSPISpeed The requested max SPI speed. The board typically picks the 
	 *				smaller of this and it's own max.
	 */
	BaseLEDMatrix(
			unsigned int rows,
			unsigned int columns,
			unsigned int columnBitWidth,
			unsigned int pwmCycleScanCount,
			bool columnControlBitOn = LOW,
			bool rowControlBitOn = LOW,
			unsigned int interFrameOffTimeMicros = 0,
#if defined ( ESP8266 )
			int slavePin = D8	
#else
			int slavePin = 10	
#endif
			,
			unsigned long maxSPISpeed = 18000000
		);
	virtual ~BaseLEDMatrix();
	
	/**
	 * Should be called before any operations against this object is performed. Child
	 * classes implementing matrix-type specific implementation should call this parent 
	 * class implementation of setup() before doing their own setup work.
	 */
	virtual void setup();  

	/**
	 * Increments the draw lock. While a matrix has a non-zero draw lock, any changes to 
	 * the image() buffer will not be pass through to the matrix scan buffer.
	 */
	void startDrawing(void)   			{ _isDrawingCount++; }
	
	/**
	 * Decrements the draw lock. All calls to startDrawing() should be balanced with a 
	 * call to stopDrawing().
	 */
	void stopDrawing(void)    			{ _isDrawingCount--; if (_isDrawingCount < 0) { _isDrawingCount = 0; }}
	
	
	/**
	 * Returns true is a draw lock is active.
	 */
	bool isDrawing(void) const			{ return (_isDrawingCount > 0); }
	
	/**
	 * Returns the number of rows in this matrix.
	 *
	 * @return the number of rows in the matrix.
	 */
	unsigned int rows() const          			{ return _rows; }

	/**
	 * Returns the number of columns in this matrix.
	 *
	 * @return the number of columns in the matrix.
	 */
	unsigned int columns() const       			{ return _columns; }

	/**
	 * Begins the row scan timer interrupt, which in turn starts sending data through the 
	 * SPI interface to the LED matrix.
	 */
	void startScanning(void);

	/**
	 * Stops the LED matrix row scan timmer interrupt.
	 */
	void stopScanning(void);

	/**
	 * This methods are "private" to this class but have to be declared public so 
	 * the timer interrupt can access them.
	 */
	bool doInterFrameTransmitOff( void ) const;
	void shiftOutCurrentRow(void);
	void shiftOutAllOff(void);
	unsigned int nextRowScanTimerInterval(void) const;
	unsigned int rowOffTimerInterval(void) const;
	void incrementScanRow( void );
};

#endif //__SCREEN_H__

