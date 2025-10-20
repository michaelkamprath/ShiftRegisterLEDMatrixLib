// Arduino Due (SAM3X) timer handlers for Shift Register LED Matrix
#ifdef SRLM_PLATFORM_IMPL
#if defined(_SAM3XA_)
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
	return 55*this->baseIntervalMultiplier(_scanPass);
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
		gSingleton->shiftOutCurrentControlRow();

		TC_SetRC(TC2, 1, gSingleton->nextRowScanTimerInterval());

		NVIC_ClearPendingIRQ(TC7_IRQn);
		NVIC_EnableIRQ(TC7_IRQn);
		TC_Start(TC2, 1);

		gSingleton->incrementScanRow();
	}
}
#endif // defined(_SAM3XA_)
#endif // SRLM_PLATFORM_IMPL
