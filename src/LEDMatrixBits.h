//     Shift Register LED Matrix Project - LEDMatrixBits
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
#ifndef __LEDMATRIXBITTS_H__
#define __LEDMATRIXBITTS_H__
// Represents bits of each row of a LED matrix controlled by 74HC595 shift registers.
// The MSB is column 0 and LSB is row 0. New column+row control bits for each row starts
// on a byte boundary.
// If any padding is needed, that must be accounted for by user, typically via extra columns.
//


#include <Arduino.h>
#include "SPIConnection.h"

class LEDMatrixBits {
private:
	size_t _dataByteCount;
	unsigned char *_data;
	size_t _columns;
	size_t _rows;
	size_t _controlBitBytesPerRow;
	bool _columnControlBitOn;
	bool _rowControlBitOn;
	
	bool* _rowMemoized;
	
	void setNBitsTo( size_t startBit, size_t numBits, unsigned char highOrLow );
public:

	LEDMatrixBits(
			size_t rows,
			size_t columns,
			bool columnControlBitOn = LOW,
			bool rowControlBitOn = LOW
		);
		
	virtual ~LEDMatrixBits();

	void reset(void);
	
	size_t rows(void) const			{ return _rows; }
	size_t columns(void) const		{ return _columns; }
	
	void setAllOff(void);
	void setColumnControlBit( size_t row, size_t column, bool isOn );
	void setRowControlBit( size_t row, bool isOn );
	
	bool isRowMemoized(int row) const;
	void unMemoizeRow(int row);
	
	void transmitRow(int row, SPIConnection& conn) const;
	
	// debug
	void streamToSerial(void);
};

#endif // __LEDMATRIXBITTS_H__
