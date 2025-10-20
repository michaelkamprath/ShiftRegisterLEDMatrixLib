// Arduino Zero (SAMD) timer handlers for Shift Register LED Matrix
#ifdef SRLM_PLATFORM_IMPL
#if defined(ARDUINO_SAMD_ZERO)
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
	return 59*this->baseIntervalMultiplier(_scanPass);
}

void BaseLEDMatrix::stopScanning(void) {
	TcCount16* TC = (TcCount16*) TC3; // get timer struct
	TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC
	while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

void TC3_Handler() {
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
		gSingleton->shiftOutCurrentControlRow();
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
#endif // defined(ARDUINO_SAMD_ZERO)
#endif // SRLM_PLATFORM_IMPL
