# Arduino Uno R4 Support Notes

The Renesas RA4M1 that powers the Arduino Uno R4 family now has a dedicated timer backend inside [`BaseLEDMatrix.cpp`](../src/BaseLEDMatrix.cpp). The implementation mirrors the behaviour of the other architecture-specific drivers while relying on the Renesas core's [`FspTimer`](https://github.com/arduino/ArduinoCore-renesas) helper to keep the scan cadence in microsecond units.

## Timer integration

* The Uno R4 handler lives behind `#elif defined(ARDUINO_ARCH_RENESAS)` alongside the existing AVR, SAMD, SAM3X, ESP, and Teensy backends.【F:src/BaseLEDMatrix.cpp†L560-L642】
* `FspTimer` exposes a periodic interrupt in microseconds, allowing the library to keep using 5 µs PWM slots by returning `5 * baseIntervalMultiplier()` from `nextRowScanTimerInterval()`.【F:src/BaseLEDMatrix.cpp†L605-L639】
* The callback replicates the established ISR flow: stop the timer, optionally shift an all-off frame, queue the next scan period, and restart the timer before advancing the scan row counters. The callback is marked with `__attribute__((__used__))` so that link-time optimisation in the Renesas toolchain keeps it alive.【F:src/BaseLEDMatrix.cpp†L574-L606】

## SPI considerations

The Uno R4's SPI peripheral is comfortable at 8 MHz, so the examples' default `maxSPISpeed` continues to work without adjustment. If your hardware includes especially long or noisy wiring, lower the clock passed to the `RGBLEDMatrix` constructor to trade throughput for signal integrity.【F:src/SPIConnection.cpp†L33-L57】

## Hardware notes

The Uno R4 uses 3.3 V logic. Continue to level-shift the SPI and latch lines when driving 5 V LED matrices, just as you would with other 3.3 V boards described in the README.

## Working with the branch

These notes and the implementation that accompanies them live on the `feature/uno-r4` branch. The automated review environment cannot push branches to your GitHub repository, so after checking out this workspace locally you will need to publish it yourself:

```sh
git checkout feature/uno-r4
git push -u origin feature/uno-r4
```

Once pushed, the branch will appear on GitHub where you can open a pull request or test directly on hardware.
