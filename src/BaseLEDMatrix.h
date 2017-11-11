//     RGB Matrix Project - BaseLEDMatrix
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

#ifndef __BASELEDMATRIX_H__
#define __BASELEDMATRIX_H__
#include <Arduino.h>
#include "TimerAction.h"
#include "LEDMatrixBits.h"
#include "SPIConnection.h"

#define LED_MATRIX_MAX_SCAN_PASS_COUNT 1

class BaseLEDMatrix : public TimerAction {
	
private:
	unsigned int _rows;
	unsigned int _columns;
	unsigned int _columnBitWidth;
	unsigned int _pwmCycleScanCount;
	bool _columnControlBitOn;
	bool _rowControlBitOn;
		
	LEDMatrixBits **_curScreenBitFrames;
	LEDMatrixBits **_screenBitFrames;
	bool _screenBitFrameToggle;
	
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
	
	virtual unsigned int multiplier5microseconds( size_t frame ) const;
public:
  

	BaseLEDMatrix(
			unsigned int rows,
			unsigned int columns,
			unsigned int columnBitWidth,
			unsigned int pwmCycleScanCount,
			bool columnControlBitOn = LOW,
			bool rowControlBitOn = LOW,
#if defined ( ESP8266 )
			int slavePin = D8	
#else
			int slavePin = 10	
#endif
		);
	virtual ~BaseLEDMatrix();
	
	virtual void setup();  

	void startDrawing(void)   			{ _isDrawingCount++; }
	void stopDrawing(void)    			{ _isDrawingCount--; if (_isDrawingCount < 0) { _isDrawingCount = 0; }}
	bool isDrawing(void) const			{ return (_isDrawingCount > 0); }
	
	unsigned int rows() const          			{ return _rows; }
	unsigned int columns() const       			{ return _columns; }

	void startScanning(void);
	void stopScanning(void);

	void shiftOutCurrentRow(void);
	unsigned int nextTimerInterval(void) const;
	void incrementScanRow( void );
};

#endif //__SCREEN_H__

