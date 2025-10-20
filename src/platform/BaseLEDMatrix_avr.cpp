// AVR (ATmega 8-bit) timer handlers for Shift Register LED Matrix
#ifdef SRLM_PLATFORM_IMPL
#if defined(ARDUINO_ARCH_AVR) || defined(__AVR__)
//
// On normal AVR Arduino board (Uno R3, Nano, etc), use the timer interrupts to drive the
// scan timing.
//

// Base timer interval units for ATmega Timer2 with /32 prescaler
// This yields approximately 50 microsecond intervals on a 16 MHz chip
#define BASE_SCAN_TIMER_INTERVALS 24

void BaseLEDMatrix::startScanning(void) {
	this->setup();

	// Maximum 8-bit timer counter value for Timer2
	const unsigned int maxTimerPreload = 255u;
	if (_interFrameOffTimeMicros >= maxTimerPreload) {
		_interFrameOffTimeInterval = 0u;
	} else {
		_interFrameOffTimeInterval = maxTimerPreload - _interFrameOffTimeMicros;
	}

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
	// This yields multiples of 50 microseconds on a 16 MHz chip
	// Max interval is 257 due to 8-bit counter (256) plus 1 for rollover
	const unsigned int maxInterval = 257u;
	const unsigned int interval = this->baseIntervalMultiplier(_scanPass) * BASE_SCAN_TIMER_INTERVALS;
	if (interval >= maxInterval) {
		return 0u;
	}
	return maxInterval - interval;
}

ISR(TIMER2_OVF_vect) {
	noInterrupts(); // disable all interrupts
	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();
		TCNT2 = gSingleton->rowOffTimerInterval();
		interrupts();
	} else {
		// shift out next row
		gSingleton->shiftOutCurrentControlRow();

		// reload the timer
		TCNT2 = gSingleton->nextRowScanTimerInterval();
		interrupts(); // enable all interrupts

		// update scan row. Done outside of interrupt stoppage since execution time can
		// be inconsistent, which would lead to vary brightness in rows.
		gSingleton->incrementScanRow();
	}
}
#endif // defined(ARDUINO_ARCH_AVR) || defined(__AVR__)
#endif // SRLM_PLATFORM_IMPL
