// ESP8266-specific timer handlers for Shift Register LED Matrix
#ifdef SRLM_PLATFORM_IMPL
#if defined(ESP8266)
//
// On the ESP8266 boards, use the timer0 to drive scan timing
// Use D5 (GPIO14) as CLK and D7 (CPIO13) as SER
//
inline void timer0InteruptHandler(void) {
	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();

		// reload the timer
		// ESP8266 runs at 84MHz, so multiply by 84 to convert microseconds to CPU cycles
		timer0_write(ESP.getCycleCount() + 84*gSingleton->rowOffTimerInterval());
		interrupts();
	} else {
		gSingleton->shiftOutCurrentControlRow();

		// reload the timer
		// ESP8266 runs at 84MHz, so multiply by 84 to convert microseconds to CPU cycles
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

	// Ensure we don't miss the first ISR call upon start up
	// Convert microseconds to CPU cycles (84 cycles per microsecond at 84MHz)
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

SRLM_ISR_ATTR unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// ESP8266 uses 5 microsecond base intervals
	// Calculates the microseconds for each scan
	return 5*this->baseIntervalMultiplier(_scanPass);
}
#endif // defined(ESP8266)
#endif // SRLM_PLATFORM_IMPL
