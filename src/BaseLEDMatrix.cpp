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

#include "BaseLEDMatrix.h"
#include "SRLEDMatrixUtils.h"

const unsigned long UPDATE_INTERVAL = 2000;

static BaseLEDMatrix* gSingleton = NULL;

BaseLEDMatrix::BaseLEDMatrix( 
	unsigned int rows,
	unsigned int columns,
	unsigned int columnBitWidth,
	unsigned int pwmCycleScanCount,
	bool columnControlBitOn,
	bool rowControlBitOn,
	unsigned int interFrameOffTimeMicros,
	int slavePin,
	unsigned long maxSPISpeed
) :		TimerAction(UPDATE_INTERVAL),
		_rows(rows),
		_columns(columns),
		_columnBitWidth(columnBitWidth),
		_pwmCycleScanCount(pwmCycleScanCount),
		_interFrameOffTimeMicros(interFrameOffTimeMicros),
		_columnControlBitOn(columnControlBitOn),
		_rowControlBitOn(rowControlBitOn),
		_curScreenBitFrames(NULL),
		_screenBitFrames(new LEDMatrixBits*[pwmCycleScanCount*2]),
		_allOffBits(NULL),
		_screenBitFrameToggle(false),
		_interFrameTransmitOffToggle(false),
		_scanPass(1),
		_scanRow(0),
		_isDrawingCount(0),
		_spi(slavePin, maxSPISpeed)
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

		if (_interFrameOffTimeMicros > 0) {
			_allOffBits = new LEDMatrixBits(
											this->rows(),
											this->columns()*_columnBitWidth,
											_columnControlBitOn,
											_rowControlBitOn
										);
			_allOffBits->setAllOff();
		}
		
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

ICACHE_RAM_ATTR bool BaseLEDMatrix::doInterFrameTransmitOff( void ) const {
	return _interFrameTransmitOffToggle;
}
ICACHE_RAM_ATTR void BaseLEDMatrix::shiftOutAllOff(void) {
	_allOffBits->transmitRow(_scanRow, _spi);
	_interFrameTransmitOffToggle = false;
}
ICACHE_RAM_ATTR void BaseLEDMatrix::shiftOutCurrentRow( void ) {
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
	
	if (_interFrameOffTimeMicros > 0) {
		_interFrameTransmitOffToggle = true;
	}
}

// Number of 5 microsecond units
ICACHE_RAM_ATTR unsigned int BaseLEDMatrix::baseIntervalMultiplier( size_t frame ) const {
	// base case does nothing interesting
	return  1;
}

ICACHE_RAM_ATTR unsigned int BaseLEDMatrix::rowOffTimerInterval(void) const {
	// _interFrameOffTimeInterval should be set in the platform-specific startScanning() 
	// method
	return  _interFrameOffTimeInterval;
}

/*
 * Interrupt Handlers
 *
 * Here, different interrupt handlers are implemented for each kind of micro-controller.
 * Pull requests for more microcontroller types are encouraged!
 *
 * The basic goal of the handlers is to fire the next interrupt N*5 microseconds after
 * the last one interrupt ended, where N is multiple determine by the scan count. This 
 * requires stopping then starting the interrupts within the handler and ensure what 
 * happens while interrupts are off takes a consistent number of clock cycles, otherwise 
 * the LEDs will have uneven brightness.
 *
 */
 
#pragma mark Teensy Handlers
#if (defined(__arm__) && defined(TEENSYDUINO))
//
// On the Teensy ARM boards, use the TimerThree library to drive scan timing
//
#include <TimerThree.h>

void time3InteruptHandler( void ) {
	Timer3.stop();
	
	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();
		
		// reload the timer
		Timer3.setPeriod(gSingleton->rowOffTimerInterval());
		Timer3.start(); 
	} else {
		gSingleton->shiftOutCurrentRow();
		// reload the timer
		Timer3.setPeriod(gSingleton->nextRowScanTimerInterval());
		Timer3.start(); 
		// update scan row. Done outside of interrupt stoppage since execution time can
		// be inconsistent, which would lead to vary brightness in rows.
		gSingleton->incrementScanRow();
	} 
}

void BaseLEDMatrix::startScanning(void) {
	this->setup();
	
	_interFrameOffTimeInterval = _interFrameOffTimeMicros;
	
	Timer3.initialize(this->nextRowScanTimerInterval());
	Timer3.attachInterrupt(time3InteruptHandler);
	Timer3.start();
}

void BaseLEDMatrix::stopScanning(void) {
	Timer3.stop();
	Timer3.detachInterrupt();
}

unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// Calculates the microseconds for each scan	
	return  5*this->baseIntervalMultiplier( _scanPass );
}

#pragma mark ESP8266 Handlers
#elif defined ( ESP8266 )

//
// On the ESP8266 boards, use the timer0 to drive scan timing
// Use D5 (GPIO14) as CLK and D7 (CPIO13) as SER
//
inline void timer0InteruptHandler (void){
	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();
		
		// reload the timer
		timer0_write(ESP.getCycleCount() + 84*gSingleton->rowOffTimerInterval());		
		interrupts();
	} else {
		gSingleton->shiftOutCurrentRow();
		
		// reload the timer
		timer0_write(ESP.getCycleCount() + 84*gSingleton->nextRowScanTimerInterval());
		interrupts();
		
		// update scan row. Done outside of interrupt stoppage since execution time can
		// be inconsistent, which would lead to vary brightness in rows.
		gSingleton->incrementScanRow();
	} 
}

void BaseLEDMatrix::startScanning(void) {
	static bool firstCall = true;

	this->setup();
	
	_interFrameOffTimeInterval = _interFrameOffTimeMicros;
	
	noInterrupts();
	timer0_isr_init();
	timer0_attachInterrupt(timer0InteruptHandler);
	
	// dirty hack to make sure we don't miss the first ISR call upon start up
	uint32_t tickCount = ESP.getCycleCount() + 84 * this->nextRowScanTimerInterval();
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

ICACHE_RAM_ATTR unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// Calculates the microseconds for each scan
	return  5*this->baseIntervalMultiplier( _scanPass );
}

#pragma mark Arduino Due Handlers
#elif defined(_SAM3XA_) // Arduino Due

void BaseLEDMatrix::startScanning(void) {
	this->setup();
	
	_interFrameOffTimeInterval = 11*_interFrameOffTimeMicros;

	/* turn on the timer clock in the power management controller */
	pmc_set_writeprotect(false);		 // disable write protection for pmc registers
	pmc_enable_periph_clk(ID_TC7);	 // enable peripheral clock TC7

	/* we want wavesel 01 with RC */
	TC_Configure(/* clock */ TC2,/* channel */ 1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2); 
	TC_SetRC(TC2, 1, 1000);
	TC_Start(TC2, 1);

	// enable timer interrupts on the timer
	TC2->TC_CHANNEL[1].TC_IER=TC_IER_CPCS;   // IER = interrupt enable register
	TC2->TC_CHANNEL[1].TC_IDR=~TC_IER_CPCS;  // IDR = interrupt disable register

	/* Enable the interrupt in the nested vector interrupt controller */
	/* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1 */
	NVIC_EnableIRQ(TC7_IRQn);

}

unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// Calculates the microseconds for each scan
	// The base interval is set to 55, which for the 
	// 10.5MHz CLOCK2 yields a ~5 micro second interval.
	return  55*this->baseIntervalMultiplier( _scanPass );
}

void BaseLEDMatrix::stopScanning(void) {
 	NVIC_DisableIRQ(TC7_IRQn);	
	TC_Stop(TC2, 1);
}

void TC7_Handler() {
	
 	TC_GetStatus(TC2, 1);
 	NVIC_DisableIRQ(TC7_IRQn);	
	TC_Stop(TC2, 1);


	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();
		
		TC_SetRC(TC2, 1, gSingleton->rowOffTimerInterval()); 

		NVIC_ClearPendingIRQ(TC7_IRQn);
		NVIC_EnableIRQ(TC7_IRQn);
		TC_Start(TC2, 1);
	} else {
		gSingleton->shiftOutCurrentRow();

		TC_SetRC(TC2, 1, gSingleton->nextRowScanTimerInterval()); 

		NVIC_ClearPendingIRQ(TC7_IRQn);
		NVIC_EnableIRQ(TC7_IRQn);
		TC_Start(TC2, 1);

		gSingleton->incrementScanRow();
	} 
}

#pragma mark Arduino Zero Handlers
#elif defined(ARDUINO_SAMD_ZERO) // Arduino Zero

void BaseLEDMatrix::startScanning(void) {
	this->setup();

	_interFrameOffTimeInterval = 12*_interFrameOffTimeMicros;

	REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3) ;
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync 

	// The type cast must fit with the selected timer mode 
	TcCount16* TC = (TcCount16*) TC3; // get timer struct

	TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

	TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;  // Set Timer counter Mode to 16 bits
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
	TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_NFRQ; // Set TC as  match mode 
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

	TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV4;   // Set perscaler
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

	TC->CC[0].reg = 0xFFFF;
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

	// Interrupts 
	TC->INTENSET.reg = 0;              // disable all interrupts
	TC->INTENSET.bit.MC0 = 1;          // enable compare match to CC0

	// Enable InterruptVector
	NVIC_EnableIRQ(TC3_IRQn);

	// 	Enable TC
	TC->CTRLA.reg |= TC_CTRLA_ENABLE;
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
}

unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// Calculates the microseconds for each scan
	// The base interval is set to 59, which with a /4
	// prescaler should yield a 5 ms interval.
	return  59*this->baseIntervalMultiplier( _scanPass );
}


void BaseLEDMatrix::stopScanning(void) {
	TcCount16* TC = (TcCount16*) TC3; // get timer struct
	TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
}

void TC3_Handler()                              // Interrupt Service Routine (ISR) for timer TC4
{     
	TcCount16* TC = (TcCount16*) TC3; // get timer struct

	TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();

		// reload the timer
		TC->CC[0].reg = gSingleton->rowOffTimerInterval();
		while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
		
		TC->INTFLAG.bit.MC0 = 1;
	
		TC->CTRLA.reg |= TC_CTRLA_ENABLE;
		while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
	} else {
		// shift out next row
		gSingleton->shiftOutCurrentRow();
		// reload the timer
		TC->CC[0].reg = gSingleton->nextRowScanTimerInterval();
		while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

		TC->INTFLAG.bit.MC0 = 1;
	
		TC->CTRLA.reg |= TC_CTRLA_ENABLE;
		while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

		// update scan row. Done outside of interrupt stoppage since execution time can
		// be inconsistent, which would lead to vary brightness in rows.
		gSingleton->incrementScanRow();
	} 
}

#pragma mark ATmega 8-bit Handlers
#else
//
// On normal Arduino board (Uno, Nano, etc), use the timer interrupts to drive the
// scan timing. 
//

#define BASE_SCAN_TIMER_INTERVALS 24

void BaseLEDMatrix::startScanning(void) {
	this->setup();
	
	_interFrameOffTimeInterval = max(255 - _interFrameOffTimeMicros, 0);

	noInterrupts(); // disable all interrupts

	TIMSK2 &= ~(1<<TOIE2); // disable timer overflow interupt

	// SET timer2 to count up mode
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);
	// set clock to I/O clock
	ASSR &= ~(1<<AS2);

	// overflow only mode
	TIMSK2 &= ~(1<<OCIE2A);

	// configure prescaler to /32 
	TCCR2B |= (1<<CS20);
	TCCR2B |= (1<<CS21);
	TCCR2B &= ~(1<<CS22) ; 

	// load counter start point and enable the timer
	TCNT2 = 0; // max interval for first timer fire
	TIMSK2 |= (1<<TOIE2);
	
  	interrupts(); // enable all interrupts
}

void BaseLEDMatrix::stopScanning(void) {
  	TIMSK2 &= ~(1<<TOIE2); // disable timer overflow interupt
}

unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// this yields multiple of 50 microseconds on a 16 MHz chip
	return  max(257 - this->baseIntervalMultiplier( _scanPass )*BASE_SCAN_TIMER_INTERVALS, 0 );
}

ISR(TIMER2_OVF_vect) {
	noInterrupts(); // disable all interrupts
	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();
		
		TCNT2 = gSingleton->rowOffTimerInterval();
		
		interrupts();
	} else {
		// shift out next row
		gSingleton->shiftOutCurrentRow();
	
		// reload the timer
		TCNT2 = gSingleton->nextRowScanTimerInterval();	
		interrupts(); // enable all interrupts
	
		// update scan row. Done outside of interrupt stoppage since execution time can
		// be inconsistent, which would lead to vary brightness in rows.
		gSingleton->incrementScanRow();
	} 
}
#endif
