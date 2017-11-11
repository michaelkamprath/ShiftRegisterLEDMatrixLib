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

#include "BaseLEDMatrix.h"
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif

#define SPICACHEPADBITS(rows,cols)	(rows*3 + cols)%8 ? 8 - (rows*3 + cols)%8 : 0
#define SPICACHESIZE(rows,cols)	1+((rows*3 + cols)-1)/8

const unsigned long UPDATE_INTERVAL = 2000;

static BaseLEDMatrix* gSingleton = NULL;

BaseLEDMatrix::BaseLEDMatrix( 
	unsigned int rows,
	unsigned int columns,
	unsigned int columnBitWidth,
	unsigned int pwmCycleScanCount,
	bool columnControlBitOn,
	bool rowControlBitOn,
	int slavePin
) :		TimerAction(UPDATE_INTERVAL),
		_rows(rows),
		_columns(columns),
		_columnBitWidth(columnBitWidth),
		_pwmCycleScanCount(pwmCycleScanCount),
		_columnControlBitOn(columnControlBitOn),
		_rowControlBitOn(rowControlBitOn),
		_curScreenBitFrames(NULL),
		_screenBitFrames(new LEDMatrixBits*[pwmCycleScanCount*2]),
		_screenBitFrameToggle(false),
		_scanPass(1),
		_scanRow(0),
		_isDrawingCount(0),
		_spi(slavePin)
{

}

void BaseLEDMatrix::setup() {
	if (_curScreenBitFrames == NULL) {
		for (unsigned int i = 0; i < 2*_pwmCycleScanCount; i++) {
			_screenBitFrames[i] = new LEDMatrixBits(
										this->rows(),
										this->columns()*_columnBitWidth,
										_columnControlBitOn,
										_rowControlBitOn
									);
		}

		_curScreenBitFrames = &_screenBitFrames[0];

		if (gSingleton == nullptr) {
			gSingleton = this;
		}
		
		_spi.setup();
	}
}

BaseLEDMatrix::~BaseLEDMatrix() {
	if (_screenBitFrames != NULL) {
		for (unsigned int i = 0; i < 2*_pwmCycleScanCount; i++) {
			delete _screenBitFrames[i];
		}
		delete _screenBitFrames;
		_curScreenBitFrames = NULL;
	}
}

void BaseLEDMatrix::action() {    
	// if the matrix need to update the frame bits and there is no drawing happening,
	// rebuild the frame bits.
	if (!this->isDrawing() ) {
		if (this->matrixNeedsUpdate()) {
			size_t idxOffset = 0;
			if (!_screenBitFrameToggle) {
				idxOffset = _pwmCycleScanCount;
			}
			
			for (size_t i = 0;  i < _pwmCycleScanCount; i++) {
				_screenBitFrames[i+idxOffset]->reset();		
				this->generateFrameBits(*_screenBitFrames[i+idxOffset], i+1);
			}
	
			noInterrupts(); // disable all interrupts
			_screenBitFrameToggle = !_screenBitFrameToggle;
			_curScreenBitFrames = &_screenBitFrames[0+idxOffset];
			interrupts(); // enable all interrupts

			this->matrixHasBeenUpdated();
		}
	} 
}

void BaseLEDMatrix::shiftOutCurrentRow( void ) {
	this->shiftOutRow( _scanRow, _scanPass );
}

ICACHE_RAM_ATTR void BaseLEDMatrix::shiftOutRow( int row, int scanPass ) {
	_curScreenBitFrames[scanPass-1]->transmitRow(row, _spi);
}

ICACHE_RAM_ATTR void BaseLEDMatrix::incrementScanRow( void ) {	
	_scanRow++;
	if (_scanRow >= this->rows()) {
		_scanRow = 0;
		_scanPass++;
		if (_scanPass > _pwmCycleScanCount) {
			_scanPass = 1;
		}
	}
}

// Number of 5 microsecond units
ICACHE_RAM_ATTR unsigned int BaseLEDMatrix::multiplier5microseconds( size_t frame ) const {
	// base case does nothing interesting
	return  1;
}

#if (defined(__arm__) && defined(TEENSYDUINO))
//
// On the Teensy ARM boards, use the TimerThree library to drive scan timing
//
#include <TimerThree.h>

void time3InteruptHandler( void ) {
	Timer3.stop();
	gSingleton->shiftOutCurrentRow();
	// reload the timer
	Timer3.setPeriod(gSingleton->nextTimerInterval());
  	Timer3.start(); 
  	// update scan row. Done outside of interrupt stoppage since execution time can
  	// be inconsistent, which would lead to vary brightness in rows.
  	gSingleton->incrementScanRow();
	
}

void BaseLEDMatrix::startScanning(void) {
	this->setup();
	
	Timer3.initialize(this->nextTimerInterval());
	Timer3.attachInterrupt(time3InteruptHandler);
	Timer3.start();
}

void BaseLEDMatrix::stopScanning(void) {
	Timer3.stop();
	Timer3.detachInterrupt();
}

unsigned int BaseLEDMatrix::nextTimerInterval(void) const {
	// Calculates the microseconds for each scan	
	return  5*this->multiplier5microseconds( _scanPass );
}


#elif defined ( ESP8266 )

//
// On the ESP8266 boards, use the timer0 to drive scan timing
// Use D5 (GPIO14) as CLK and D7 (CPIO13) as SER
//
inline void timer0InteruptHandler (void){
	gSingleton->shiftOutCurrentRow();
	// reload the timer
 	timer0_write(ESP.getCycleCount() + 84 * gSingleton->nextTimerInterval());
  	// update scan row. Done outside of interrupt stoppage since execution time can
  	// be inconsistent, which would lead to vary brightness in rows.
	interrupts();
  	gSingleton->incrementScanRow();
}

void BaseLEDMatrix::startScanning(void) {
	static bool firstCall = true;

	this->setup();
	
	noInterrupts();
	timer0_isr_init();
	timer0_attachInterrupt(timer0InteruptHandler);
	
	// dirty hack to make sure we don't miss the first ISR call upon start up
	uint32_t tickCount = ESP.getCycleCount() + 84 * this->nextTimerInterval();
	if (firstCall) {
		tickCount += 5000;
		firstCall = false;
	}
 	timer0_write(tickCount);
	interrupts();
}

void BaseLEDMatrix::stopScanning(void) {
	timer0_detachInterrupt();
}

ICACHE_RAM_ATTR unsigned int BaseLEDMatrix::nextTimerInterval(void) const {
	// Calculates the microseconds for each scan
	return  5*this->multiplier5microseconds( _scanPass );
}

#elif defined(ARDUINO_SAMD_ZERO)||defined(_SAM3XA_)
// no timer code for the Due or Zero yet

#else
//
// On normal Arduino board (Uno, Nano, etc), use the timer interrupts to drive the
// scan timing. 
//

#define BASE_SCAN_TIMER_INTERVALS 12

void BaseLEDMatrix::startScanning(void) {
	this->setup();

	noInterrupts(); // disable all interrupts
	
  	TIMSK2 &= ~(1<<TOIE2); // disable timer overflow interupt

	// SET timer2 to count up mode
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);

	// set clock to I/O clock
  	ASSR &= ~(1<<AS2);
 
  	// overflow only mode
  	TIMSK2 &= ~(1<<OCIE2A);
  	
  	// configure to fire about every 5 micro-second 
	TCCR2B |= (1<<CS22) ; 
	TCCR2B &= ~(1<<CS20);
	TCCR2B &= ~(1<<CS21);

	// load counter start point and enable the timer
	TCNT2 = this->nextTimerInterval();
	TIMSK2 |= (1<<TOIE2);
	
  	interrupts(); // enable all interrupts
}

void BaseLEDMatrix::stopScanning(void) {
  	TIMSK2 &= ~(1<<TOIE2); // disable timer overflow interupt
}

unsigned int BaseLEDMatrix::nextTimerInterval(void) const {
	return  max(257 - this->multiplier5microseconds( _scanPass )*BASE_SCAN_TIMER_INTERVALS, 0 );
}


ISR(TIMER2_OVF_vect) {
	noInterrupts(); 
	// shift out next row
	gSingleton->shiftOutCurrentRow();
	// reload the timer
	TCNT2 = gSingleton->nextTimerInterval();
  	interrupts(); 
  	// update scan row. Done outside of interrupt stoppage since execution time can
  	// be inconsistent, which would lead to vary brightness in rows.
  	gSingleton->incrementScanRow();
}
#endif
