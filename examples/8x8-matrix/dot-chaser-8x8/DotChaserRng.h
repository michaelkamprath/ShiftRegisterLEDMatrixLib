#ifndef DOT_CHASER_RNG_H
#define DOT_CHASER_RNG_H

#include <Arduino.h>

// The Renesas-based Uno R4 implements random() by hitting the on-chip hardware RNG,
// which introduces visible stalls when the dot chaser examples bounce at the screen
// edges. Keep the animation smooth by using a lightweight xorshift32 generator instead.
namespace DotChaserRng {
	static uint32_t state = 0x1D2F3B4Cu;

	inline void seed(uint32_t value) {
		if (value != 0u) {
			state = value;
		}
	}

	inline uint32_t nextValue(void) {
		uint32_t x = state;
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		state = x;
		return x;
	}

	inline unsigned int nextIndex(unsigned int limit) {
		if (limit == 0u) {
			return 0u;
		}
		return static_cast<unsigned int>(nextValue() % limit);
	}

	inline int nextNonZeroStep(void) {
		int value = static_cast<int>(nextValue() % 3u) - 1;
		if (value == 0) {
			value = (nextValue() & 1u) ? 1 : -1;
		}
		return value;
	}
}

#endif // DOT_CHASER_RNG_H
