// ESP32-specific timer handlers for Shift Register LED Matrix
#ifdef SRLM_PLATFORM_IMPL
#if defined(ESP32)
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
	portENTER_CRITICAL_ISR(&timerMux);
	timerStop(timer);
	if (gSingleton->doInterFrameTransmitOff()) {
		gSingleton->shiftOutAllOff();
	} else {
		gSingleton->shiftOutCurrentControlRow();
		// update scan row. Done outside of interrupt stoppage since execution time can
		// be inconsistent, which would lead to vary brightness in rows.
		gSingleton->incrementScanRow();
	}
	timerAlarmWrite(timer, gSingleton->nextRowScanTimerInterval(), true);
	timerRestart(timer);
	portEXIT_CRITICAL_ISR(&timerMux);
}

void BaseLEDMatrix::startScanning(void) {
	this->setup();

	_interFrameOffTimeInterval = _interFrameOffTimeMicros;

	noInterrupts();

	// ESP32 timer configuration: Use timer 3 for best compatibility
	// Prescaler of 80 divides 80MHz clock to 1MHz (1 microsecond ticks)
	timer = timerBegin(3, 80, true);
	timerAttachInterrupt(timer, &onTimer, true);
	timerAlarmWrite(timer, this->nextRowScanTimerInterval(), true);
	timerAlarmEnable(timer);

	interrupts();
}

void BaseLEDMatrix::stopScanning(void) {
	timerEnd(timer);
}

unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// ESP32 uses 5 microsecond base intervals
	// This sets the interrupt to fire at multiples of 5 microseconds
	return 5*this->baseIntervalMultiplier(_scanPass);
}
#endif // defined(ESP32)
#endif // SRLM_PLATFORM_IMPL
