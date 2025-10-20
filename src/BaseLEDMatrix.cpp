//     Shift Register LED Matrix Project - BaseLEDMatrix
//     Copyright (C) 2017 Michael Kamprath
//
//     This file is part of Shift Register LED Matrix Project.
//
//     Shift Register LED Matrix Project is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Lesser General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Shift Register LED Matrix Project is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Lesser General Public License for more details.
//
//     You should have received a copy of the GNU Lesser General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.

#include "BaseLEDMatrix.h"
#include "SRLEDMatrixUtils.h"

// Update interval in microseconds for checking if frame bits need to be regenerated
const unsigned long UPDATE_INTERVAL = 2000;

static BaseLEDMatrix* gSingleton = NULL;

BaseLEDMatrix::BaseLEDMatrix(
	unsigned int rows,
	unsigned int columns,
	unsigned int controlRows,
	unsigned int controlColumns,
	unsigned int columnBitWidth,
	unsigned int pwmCycleScanCount,
	bool columnControlBitOn,
	bool rowControlBitOn,
	unsigned int interFrameOffTimeMicros,
	int slavePin,
	DeviceBitEndian bitEndian,
	unsigned long maxSPISpeed
) :		TimerAction(UPDATE_INTERVAL),
		_rows(rows),
		_columns(columns),
		_controlRows(controlRows),
		_controlColumns(controlColumns),
		_rowGroups(controlColumns/columns),
		_columnBitWidth(columnBitWidth),
		_pwmCycleScanCount(pwmCycleScanCount),
		_interFrameOffTimeMicros(interFrameOffTimeMicros),
		_bitEndian(bitEndian),
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
		_spi(slavePin, maxSPISpeed),
		_blankPin(-1)
{

}

void BaseLEDMatrix::setup() {
	if (_curScreenBitFrames == NULL) {
		for (unsigned int i = 0; i < 2*_pwmCycleScanCount; i++) {
			_screenBitFrames[i] = new LEDMatrixBits(
										this->controlRows(),
										this->controlColumns()*_columnBitWidth,
										_columnControlBitOn,
										_rowControlBitOn
									);
		}

		_curScreenBitFrames = &_screenBitFrames[0];

		if (_interFrameOffTimeMicros > 0) {
			_allOffBits = new LEDMatrixBits(
											this->controlRows(),
											this->controlColumns()*_columnBitWidth,
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
	this->stopScanning();
	noInterrupts();
	if (gSingleton == this) {
		gSingleton = NULL;
	}
	interrupts();

	if (_screenBitFrames != NULL) {
		for (unsigned int i = 0; i < 2*_pwmCycleScanCount; i++) {
			delete _screenBitFrames[i];
		}
		delete[] _screenBitFrames;
		_screenBitFrames = NULL;
		_curScreenBitFrames = NULL;
	}
	delete _allOffBits;
	_allOffBits = NULL;
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
				this->generateFrameBits(*_screenBitFrames[i+idxOffset], i);
			}

			noInterrupts(); // disable all interrupts
			_screenBitFrameToggle = !_screenBitFrameToggle;
			_curScreenBitFrames = &_screenBitFrames[0+idxOffset];
			interrupts(); // enable all interrupts

			this->matrixHasBeenUpdated();
		}
	}
}

SRLM_ISR_ATTR bool BaseLEDMatrix::doInterFrameTransmitOff( void ) const {
	return _interFrameTransmitOffToggle;
}
SRLM_ISR_ATTR void BaseLEDMatrix::shiftOutAllOff(void) {
	_allOffBits->transmitRow(_scanRow, _spi);
	_interFrameTransmitOffToggle = false;
}
SRLM_ISR_ATTR void BaseLEDMatrix::shiftOutCurrentControlRow( void ) {
	this->shiftOutControlRow( _scanRow, _scanPass );
}

SRLM_ISR_ATTR void BaseLEDMatrix::shiftOutControlRow( int row, int scanPass ) {
	_curScreenBitFrames[scanPass-1]->transmitRow(row, _spi);
}

SRLM_ISR_ATTR void BaseLEDMatrix::incrementScanRow( void ) {
	_scanRow++;
	if (_scanRow >= this->controlRows()) {
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

SRLM_ISR_ATTR void BaseLEDMatrix::enableBlanking(int blankPin ) {
	_blankPin = blankPin;
	_blankLevel = 0;
	pinMode (_blankPin, OUTPUT);
	digitalWrite (_blankPin, LOW);
}

SRLM_ISR_ATTR void BaseLEDMatrix::disableBlanking( void )	{
	if ( _blankPin >= 0 ) {
		digitalWrite (_blankPin, LOW);
	}
	_blankPin = -1;
	_blankLevel = 0;
}

SRLM_ISR_ATTR void BaseLEDMatrix::blank(void) {
	if ( _blankPin >= 0 ) {
		if ( _blankLevel == 0 ) {
			digitalWrite (_blankPin, HIGH);
		}
		_blankLevel++;
	}
}

SRLM_ISR_ATTR void BaseLEDMatrix::unblank(void) {
	if ((_blankPin >= 0) && (_blankLevel > 0)) {
		_blankLevel--;
		if (_blankLevel == 0) {
			digitalWrite (_blankPin, LOW);
		}
	}
}


// Number of 5 microsecond units
SRLM_ISR_ATTR unsigned int BaseLEDMatrix::baseIntervalMultiplier( size_t frame ) const {
	// base case does nothing interesting
	return  1;
}

SRLM_ISR_ATTR unsigned int BaseLEDMatrix::rowOffTimerInterval(void) const {
	// _interFrameOffTimeInterval should be set in the platform-specific startScanning()
	// method
	return  _interFrameOffTimeInterval;
}

// MARK: - Debugging
void BaseLEDMatrix::debugPrintFrames( void ) const {
	size_t idxOffset = 0;
	if (_screenBitFrameToggle) {
		idxOffset = _pwmCycleScanCount;
	}
	Serial.println("\nThe matrix frames currently are:");
	for (unsigned int i = 0; i < _pwmCycleScanCount*2; i++ ) {
		Serial.print("Frame #");
		Serial.print(i);
		if (i == idxOffset) {
			Serial.print(" (CURRENT BASE)");
		}
		Serial.print(" :\n");
		_screenBitFrames[i]->streamToSerial();
	}
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

#define SRLM_PLATFORM_IMPL

#if ((defined(__IMXRT1062__)||defined(__arm__)) && defined(TEENSYDUINO))
#include "platform/BaseLEDMatrix_teensy.cpp"
#elif defined ( ESP32 )
#include "platform/BaseLEDMatrix_esp32.cpp"
#elif defined ( ESP8266 )
#include "platform/BaseLEDMatrix_esp8266.cpp"
#elif defined(_SAM3XA_) // Arduino Due
#include "platform/BaseLEDMatrix_sam.cpp"
#elif defined(ARDUINO_SAMD_ZERO) // Arduino Zero
#include "platform/BaseLEDMatrix_samd.cpp"
#elif defined(ARDUINO_ARCH_RP2040)
#include "platform/BaseLEDMatrix_rp2040.cpp"
#elif defined(ARDUINO_ARCH_RENESAS)
#include "platform/BaseLEDMatrix_renesas_uno.cpp"
#elif defined(__AVR_ATmegax08__) || defined(__AVR_ATmegax09__)
#error This architecture is unsupported
void BaseLEDMatrix::startScanning(void) {

}

void BaseLEDMatrix::stopScanning(void) {
}

unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	return 0;
}
#else
#include "platform/BaseLEDMatrix_avr.cpp"
#endif

#undef SRLM_PLATFORM_IMPL
