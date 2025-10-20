// Arduino Uno R4 (Renesas) timer handlers for Shift Register LED Matrix
#ifdef SRLM_PLATFORM_IMPL
#if defined(ARDUINO_ARCH_RENESAS)
#include <FspTimer.h>

namespace {
FspTimer ledMatrixTimer;
bool timerInitialized = false;
// Renesas timer configuration
// Default tick interval is 5 microseconds for optimal performance
constexpr uint32_t kRenesasDefaultTickMicros = 5u;
// Maximum tick interval is 640 microseconds (hardware limitation)
constexpr uint32_t kRenesasMaxTickMicros = 640u;
volatile uint32_t renesasAccumulatedMicros = 0u;
volatile uint32_t renesasCurrentIntervalMicros = kRenesasDefaultTickMicros;
volatile uint32_t renesasTimerTickMicros = kRenesasDefaultTickMicros;
uint8_t renesasTimerType = GPT_TIMER;
int8_t renesasTimerChannel = -1;
} // namespace

__attribute__((__used__)) void renesasTimerCallback(timer_callback_args_t *args) {
	(void)args;
	if (gSingleton == nullptr) {
		return;
	}

	if (renesasTimerTickMicros == 0u) {
		return;
	}

	if (renesasCurrentIntervalMicros == 0u) {
		renesasCurrentIntervalMicros = renesasTimerTickMicros;
	}

	renesasAccumulatedMicros += renesasTimerTickMicros;

	while (renesasAccumulatedMicros >= renesasCurrentIntervalMicros) {
		renesasAccumulatedMicros -= renesasCurrentIntervalMicros;

		uint32_t nextIntervalMicros = 0u;

		if (gSingleton->doInterFrameTransmitOff()) {
			gSingleton->shiftOutAllOff();
			nextIntervalMicros = gSingleton->rowOffTimerInterval();
		} else {
			gSingleton->shiftOutCurrentControlRow();
			nextIntervalMicros = gSingleton->nextRowScanTimerInterval();
			gSingleton->incrementScanRow();
		}

		if (nextIntervalMicros == 0u) {
			nextIntervalMicros = renesasTimerTickMicros;
		}

		renesasCurrentIntervalMicros = nextIntervalMicros;
	}
}

void BaseLEDMatrix::startScanning(void) {
	this->setup();

	_interFrameOffTimeInterval = _interFrameOffTimeMicros;

	if (!timerInitialized) {
		renesasTimerChannel = FspTimer::get_available_timer(renesasTimerType, true);
		if (renesasTimerChannel < 0) {
			return;
		}

		if (!ledMatrixTimer.begin(TIMER_MODE_PERIODIC, renesasTimerType, renesasTimerChannel, 1000.0f, 50.0f, renesasTimerCallback)) {
			return;
		}

		if (!ledMatrixTimer.open()) {
			return;
		}

		uint32_t requestedTick = kRenesasDefaultTickMicros;
		bool tickConfigured = false;
		while (!tickConfigured && requestedTick <= kRenesasMaxTickMicros) {
			if (ledMatrixTimer.set_period_us(requestedTick)) {
				tickConfigured = true;
				renesasTimerTickMicros = requestedTick;
				ledMatrixTimer.stop();
			} else {
				requestedTick *= 2u;
			}
		}
		if (!tickConfigured) {
			return;
		}

		if (!ledMatrixTimer.setup_overflow_irq()) {
			return;
		}

		timerInitialized = true;
	} else {
		if (!ledMatrixTimer.is_opened()) {
			if (!ledMatrixTimer.open()) {
				return;
			}
		}
	}

	renesasAccumulatedMicros = 0u;
	renesasCurrentIntervalMicros = this->nextRowScanTimerInterval();
	if (renesasCurrentIntervalMicros == 0u) {
		renesasCurrentIntervalMicros = renesasTimerTickMicros;
	}

	if (!ledMatrixTimer.start()) {
		return;
	}
}

void BaseLEDMatrix::stopScanning(void) {
	if (timerInitialized) {
		ledMatrixTimer.stop();
	}
}

unsigned int BaseLEDMatrix::nextRowScanTimerInterval(void) const {
	// Renesas uses 5 microsecond base intervals
	// Calculates the microseconds for each scan in five microsecond units
	return 5*this->baseIntervalMultiplier(_scanPass);
}
#endif // defined(ARDUINO_ARCH_RENESAS)
#endif // SRLM_PLATFORM_IMPL
