// Teensy-specific timer handlers for Shift Register LED Matrix
#ifdef SRLM_PLATFORM_IMPL
#if ((defined(__IMXRT1062__)||defined(__arm__)) && defined(TEENSYDUINO))
#if defined(__IMXRT1062__)
// Use IntervalTimer on Teensy 4+ to drive scan timing
#include <IntervalTimer.h>

IntervalTimer ledMatrixTimer;

void timerInteruptHandler(void) {
	ledMatrixTimer.end();

	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();

		// reload the timer
		ledMatrixTimer.begin(timerInteruptHandler, gSingleton->rowOffTimerInterval());
	} else {
		gSingleton->shiftOutCurrentControlRow();

		// reload the timer
		ledMatrixTimer.begin(timerInteruptHandler, gSingleton->nextRowScanTimerInterval());
		// update scan row. Done outside of interrupt stoppage since execution time can
		// be inconsistent, which would lead to vary brightness in rows.
		gSingleton->incrementScanRow();
	}
}

void BaseLEDMatrix::startScanning(void) {
	this->setup();

	_interFrameOffTimeInterval = _interFrameOffTimeMicros;

	ledMatrixTimer.begin(timerInteruptHandler, gSingleton->nextRowScanTimerInterval());
}

void BaseLEDMatrix::stopScanning(void) {
	ledMatrixTimer.end();
}

unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// Calculates the microseconds for each scan
	return 10*this->baseIntervalMultiplier(_scanPass);
}
#else
//
// On the Teensy 3.x boards, use the TimerThree library to drive scan timing
//
#include <TimerThree.h>

void time3InteruptHandler(void) {
	Timer3.stop();

	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();

		// reload the timer
		Timer3.setPeriod(gSingleton->rowOffTimerInterval());
		Timer3.start();
	} else {
		gSingleton->shiftOutCurrentControlRow();

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
	// Teensy 3.x uses 10 microsecond base intervals
	// Calculates the microseconds for each scan
	return 10*this->baseIntervalMultiplier(_scanPass);
}
#endif // defined(__IMXRT1062__)
#endif // ((defined(__IMXRT1062__)||defined(__arm__)) && defined(TEENSYDUINO))
#endif // SRLM_PLATFORM_IMPL
