# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).


## [Unreleased]
## [1.2.0] - 2018-12-24

### Fixed
- Correct the code for the 4x4 matrix example

### Changed
- Removed ESP8266 and 8 bit AVR microcontrollers for list officially supported for 12-bit color. This was done primarily due to realistic assessment of their computational speed. 

### Added
- Support for the "Common Power Row Groups" matrix layout outs. This layout is intended for matrices with a large number of rows, too large to effectively doe a single pass scan. With this layout, rows are grouped together into row groups and corresponding rows between each group are powered together through the same switching transistor. For example, in a 16 row matrix that is split up into groups of 8, rows 1 and 9 are powered together, and so on. As a result of row groups each having a row powered simultaneously, the columns are independently controlled in each row group.  For now, the only layout supported with this scheme are ones with arow groups of size 8 and columns within each group use the RGB grouping bit layout. 
- Support for sending a `blank` single to the shift registers on demand. 

## [1.1.1] - 2018-12-24

### Changed
- Updated the Conway Game of Life example to auto-reset itself it the game evolves to a static, unchanging state.

### Added
- Basic capability to specify the endian-ness of the column layout for a matrix. While column 0 is meant to indicate the left most column, the hardware layout of the matrix might not make the left most column the MSB in the shift register chain. This might happen if you make a mistake in your hardware (oops!). By setting the column endian value of the matrix,the high level software can still reference column 0 to mean the left most column, and the bit layout sent the shift registers will be adjusted according to where column 0 physically is in the hardware.
- Added support for ESP32 microcontrollers. 

## [1.1.0] - 2018-02-09

### Changed
- Refactored the image classes to have a common, templated base class. This consolidates similar logic to a single location. This caused the existing Glyph object to have it's API slightly changed.
- Improved interrupt handling on 8-bit AVR microcontrollers

### Added
- Support for the Arduino Zero and Due boards (and related)
- Support for older ATmega8535, ATmega16 and ATmega32 microcontrollers
- Support for ATmega1284 and ATmega1284P microcontrollers
- A method to LEDImage for drawing circles
- A Red-Blue-Green bit layout mode for the RGB matrix object
- An option to shift out an "all off" signal for a short period of time in between row updates to the shift registers. This helps mitigate LED ghosting when the time to turn off for the row power switch is appreciable and cannot be mitigated in hardware. For example, when using 2981 source drivers for the row power switching, which requires 2 microseconds to turn off.
- Added support for a "gray scale" LED matrix. This is a mono-color LED matrix where PWM is used to effect varying intensity in each LED. 


## [1.0.1] - 2017-12-24
### Changed
- Adjusted the aesthetics of the classic plasma visualization example code

### Fixed
- Corrected failure to compile for boards that can only use 6-bit color.

## 1.0.0 - 2017-12-24
Initial release

[Unreleased]: https://github.com/michaelkamprath/ShiftRegisterLEDMatrixLib/compare/v1.2.0...HEAD
[1.2.0]: https://github.com/michaelkamprath/ShiftRegisterLEDMatrixLib/compare/v1.1.1...v1.2.0
[1.1.1]: https://github.com/michaelkamprath/ShiftRegisterLEDMatrixLib/compare/v1.1.0...v1.1.1
[1.1.0]: https://github.com/michaelkamprath/ShiftRegisterLEDMatrixLib/compare/v1.0.1...v1.1.0
[1.0.1]: https://github.com/michaelkamprath/ShiftRegisterLEDMatrixLib/compare/v1.0.0...v1.0.1
