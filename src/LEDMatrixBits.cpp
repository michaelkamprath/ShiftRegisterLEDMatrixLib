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
#include "LEDMatrixBits.h"
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif

const unsigned char BYTE_BIT_BITMASK[8] = {
	B10000000,
	B01000000,
	B00100000,
	B00010000,
	B00001000,
	B00000100,
	B00000010,
	B00000001
};

const unsigned char HIGH_REMAINING_BIT_PRESETS[8] = {
	B11111111,
	B01111111,
	B00111111,
	B00011111,
	B00001111,
	B00000111,
	B00000011,
	B00000001
};



#define BYTES_FOR_ROW_CONTROL_BITS(rows,cols)	(1+(rows+cols-1)/8)

LEDMatrixBits::LEDMatrixBits(
	size_t rows,
	size_t columns,
	bool columnControlBitOn,
	bool rowControlBitOn
)	: 	_dataByteCount((BYTES_FOR_ROW_CONTROL_BITS(rows,columns))*rows),
		_data(new unsigned char[BYTES_FOR_ROW_CONTROL_BITS(rows,columns)*rows]),
		_columns(columns),
		_rows(rows),
		_controlBitBytesPerRow(BYTES_FOR_ROW_CONTROL_BITS(rows,columns)),
		_columnControlBitOn(columnControlBitOn),
		_rowControlBitOn(rowControlBitOn),
		_rowMemoized(new bool[rows])
{
	this->reset();
}

LEDMatrixBits::~LEDMatrixBits()
{
	delete _data;
}

void LEDMatrixBits::reset(void) {
	this->setAllOff();
	memset(_rowMemoized, 0, this->rows());
}

bool LEDMatrixBits::isRowMemoized(int row) const {
	return _rowMemoized[row];
}
void LEDMatrixBits::setAllOff(void) {
	// first, blast everything to the column off	
	unsigned char clearValue = _columnControlBitOn ? B00000000 : B11111111;
	memset(
			_data,
			clearValue,
			_dataByteCount
		);
		
	// now set every row control bits off
	for (size_t r = 0; r < this->rows(); r++) {
		this->setRowControlBit(r, false);
	}
}


void LEDMatrixBits::setRowControlBit( size_t row, bool isOn ) {
	// get index of row control bit 0 for row
	size_t startIdx = _controlBitBytesPerRow*row;
	
	unsigned char *dataPtr = _data + startIdx + this->columns()/8;
	
	size_t curByteBit = this->columns()%8;
	
	// the last row is first, so first proper bit index
	size_t rowBit = this->rows() - row - 1;
	
	if (isOn) {
		for (size_t i = 0; i < this->rows(); i++) {

			if ( (_rowControlBitOn&&(i == rowBit)) || ((!_rowControlBitOn)&&(i != rowBit)) ) {
				*dataPtr |= BYTE_BIT_BITMASK[curByteBit];
			} else {
				*dataPtr &= ~(BYTE_BIT_BITMASK[curByteBit]);
			}		
			curByteBit++;
			if (curByteBit >= 8) {
				dataPtr++;
				curByteBit = 0;
			}
		}
	}
	else {
		//set every bit to off 
		size_t startControlBitsForRow = _controlBitBytesPerRow*8*row + this->columns();
		this->setNBitsTo(
				startControlBitsForRow,
				this->rows(),
				!_rowControlBitOn
			);
	}
	
	_rowMemoized[row] = true;
}

void LEDMatrixBits::unMemoizeRow(int row) {
	_rowMemoized[row] = false;
}


void LEDMatrixBits::setNBitsTo( size_t startBit, size_t numBits, unsigned char highOrLow ) {
	size_t startByteIdx = startBit/8;
	unsigned char *dataPtr = _data + startByteIdx;
	
	size_t bitIdxInCurByte = startBit%8;
	size_t reminingBits = numBits;
	
	while (reminingBits>0){
		if ( bitIdxInCurByte + reminingBits > 8 ) {
			if (highOrLow) {
				*dataPtr |= HIGH_REMAINING_BIT_PRESETS[bitIdxInCurByte];
			} else {
				*dataPtr &= ~HIGH_REMAINING_BIT_PRESETS[bitIdxInCurByte];
			}
			
			reminingBits -= (8-bitIdxInCurByte);
			bitIdxInCurByte = 0;
			dataPtr++;
		}
		else {
			// one bit at at a time
			if (highOrLow) {
				*dataPtr |= BYTE_BIT_BITMASK[bitIdxInCurByte];
			} else {
				*dataPtr &= ~BYTE_BIT_BITMASK[bitIdxInCurByte];
			}
			
			reminingBits--;
			bitIdxInCurByte++;
			if (bitIdxInCurByte >= 8) {
				bitIdxInCurByte = 0;
				dataPtr++;
			}
		}
		
		if (dataPtr >= 	_data + _dataByteCount) {
			// thou shalt not buffer overflow
			return;
		}
	}	
}

void LEDMatrixBits::setColumnControlBit( size_t row, size_t column, bool isOn ) {
	size_t byteIdx = _controlBitBytesPerRow*row + column/8;
	size_t bitIdxInCurByte = column%8;
	
	if (isOn == _columnControlBitOn) {
		_data[byteIdx] |= BYTE_BIT_BITMASK[bitIdxInCurByte];
	} else {
		_data[byteIdx] &= ~BYTE_BIT_BITMASK[bitIdxInCurByte];
	}
}

ICACHE_RAM_ATTR void LEDMatrixBits::transmitRow(int row, SPIConnection& conn) const {
	unsigned char *dataPtr = _data + _controlBitBytesPerRow*row;
	conn.startTransaction();
	for (size_t i = 0; i < _controlBitBytesPerRow; i++ ) {
		conn.transmitByte(*dataPtr);
		dataPtr++;
	}
	conn.endTransaction();
}

void LEDMatrixBits::streamToSerial(void) {
	unsigned char* dataPtr = _data;
	
	for (size_t row = 0; row < this->rows(); row++) {
		Serial.print(F("     "));
		size_t bitCount = 0;
		for (size_t i = 0; i < _controlBitBytesPerRow; i++) {
		   for (unsigned char mask = 0x80; mask; mask >>= 1) {
				if (mask & (*dataPtr)) {
				   Serial.print('1');
				}
				else {
				   Serial.print('0');
				}
				
				// create seperation for row control bits
				bitCount++;
				if ( bitCount == this->columns() ) {
					Serial.print(F("  "));
				}	
			}	
			dataPtr++;
		}
		Serial.print(F("\n"));		
	}
}
