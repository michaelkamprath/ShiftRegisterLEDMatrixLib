//     Shift Register LED Matrix Project - RGBColor
//     Copyright (C) 2017 Michael Kamprath
//
//     This file is part of Shift Register LED Matrix Project.
// 
//     Shift Register LED Matrix Project is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     Shift Register LED Matrix Project is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.
#ifndef __RGBCOLOR_H__
#define __RGBCOLOR_H__
#include <Arduino.h>

#ifndef TWELVE_BIT_COLOR
#define TWELVE_BIT_COLOR (defined(__arm__)&& defined(TEENSYDUINO)) \
							||defined(__AVR_ATmega2560__) \
							||defined(__AVR_ATmega1284__) \
							||defined(__AVR_ATmega1284P__) \
							||defined( ESP8266 ) \
							||defined(ARDUINO_SAMD_ZERO) \
							||defined(_SAM3XA_)
#endif

#if TWELVE_BIT_COLOR
/***

Each byte controls one LED. Each color get four bits to indicate color intensity (0-15).
The bits of the byte are laid out as follows in a 2-byte integer:

	Bits   0   4   8  12  
		   |---|---|---|---
 		   TUUURRRRGGGGBBBB

	T = transparent
	U = unused
	R = Red
	G = Green
	B = Blue

*/

typedef uint16_t RGBColorType;


const RGBColorType AQUA_COLOR = 0x00FF;
const RGBColorType BLACK_COLOR = 0;
const RGBColorType BLUE_COLOR = 0x000F;
const RGBColorType BROWN_COLOR = 0x0941;
const RGBColorType CORAL_COLOR = 0x0F75;
const RGBColorType DARK_BLUE_COLOR = 0x0004;
const RGBColorType DARK_GRAY_COLOR = 0x0444;
const RGBColorType DARK_GREEN_COLOR = 0x0040;
const RGBColorType DARK_RED_COLOR = 0x0400;
const RGBColorType GRAY_COLOR = 0x0999;
const RGBColorType GREEN_COLOR = 0x00F0;
const RGBColorType LIME_COLOR = 0x03C3;
const RGBColorType MAGENTA_COLOR = 0x0F0F;
const RGBColorType ORANGE_COLOR = 0x0FA0;
const RGBColorType PINK_COLOR = 0x0F6A;
const RGBColorType PURPLE_COLOR = 0x092E;
const RGBColorType RED_COLOR = 0x0F00;
const RGBColorType SKY_BLUE_COLOR = 0x08CF;
const RGBColorType SLATE_BLUE_COLOR = 0x065C;
const RGBColorType TRANSPARENT_COLOR = 0x8000;
const RGBColorType TURQUOISE_COLOR = 0x05FA;
const RGBColorType VIOLET_COLOR = 0x0E7E;
const RGBColorType WHITE_COLOR = 0x0FFF;
const RGBColorType YELLOW_COLOR = 0x0FF0;

const static RGBColorType RED_MASK = 0x0F00;
const static unsigned int RED_BIT_SHIFT = 8;

const static RGBColorType GREEN_MASK = 0x00F0;
const static unsigned int GREEN_BIT_SHIFT = 4;

const static RGBColorType BLUE_MASK = 0x000F;
const static unsigned int BLUE_BIT_SHIFT = 0;

#else

/***
Each byte controls one LED. Each color get two bits to indicate color intensity (0,1,2,3).
The bits of the byte are laid out as follows:
	Bits   0   4
		   |---|---
		   TURRGGBB
	T = transparent
	U = unused
	R = Red
	G = Green
	B = Blue
*/

typedef unsigned char RGBColorType;

const RGBColorType AQUA_COLOR = 0x0F;
const RGBColorType BLACK_COLOR = 0;
const RGBColorType BLUE_COLOR = 0x03;
const RGBColorType CORAL_COLOR = 0x39;
const RGBColorType DARK_BLUE_COLOR = B00000001;
const RGBColorType DARK_GRAY_COLOR = 0x15;
const RGBColorType DARK_GREEN_COLOR = B00000100;
const RGBColorType DARK_RED_COLOR = B00010000;
const RGBColorType GRAY_COLOR = 0x2A;
const RGBColorType GREEN_COLOR = 0x0C;
const RGBColorType LIME_COLOR = 0x2D;
const RGBColorType MAGENTA_COLOR = 0x33;
const RGBColorType ORANGE_COLOR = 0x38;
const RGBColorType PINK_COLOR = 0x26;
const RGBColorType PURPLE_COLOR = B00010001;
const RGBColorType RED_COLOR = 0x30;
const RGBColorType SKY_BLUE_COLOR = B00010110;
const RGBColorType SLATE_BLUE_COLOR = 0x16;
const RGBColorType TRANSPARENT_COLOR = B10000000;
const RGBColorType TURQUOISE_COLOR = 0x1E;
const RGBColorType VIOLET_COLOR = 0x27;
const RGBColorType WHITE_COLOR = 0x3F;
const RGBColorType YELLOW_COLOR = 0x3C;

const static RGBColorType RED_MASK = 0x30;
const static unsigned char RED_BIT_SHIFT = 4;

const static RGBColorType GREEN_MASK = 0x0C;
const static unsigned char GREEN_BIT_SHIFT = 2;

const static RGBColorType BLUE_MASK = 0x03;
const static unsigned char BLUE_BIT_SHIFT = 0;
#endif //TWELVE_BIT_COLOR


namespace RGBColor {

	/**
	 *	Creates an RGBColorType from a 24-bit RGB color 
	 *
	 *	@param red red value between 0 and 255
	 *	@param green green value between 0 and 255
	 *	@param blue blue value between 0 and 255
	 *  @return A RGBColorType value that closely approximates the passed color.
	 */	 
	RGBColorType fromRGB(int red, int green, int blue);
	
	
	/**
	 *	Creates an RGBColorType from a Hues,Saturation,Value color 
	 *
	 *	@param hue The hue measured in degrees
	 *	@param saturation The saturation between 0 and 1
	 *	@param value The value between 0 and 1
	 *  @return A RGBColorType value that closely approximates the passed color.
	 */	 
	RGBColorType fromHSV(float hue, float saturation, float value);
	
	/**
	 *	Creates an RGBColorType from a Hues,Saturation,Lightness color 
	 *
	 *	@param hue The hue measured in degrees
	 *	@param saturation The saturation between 0 and 1
	 *	@param lightness The lightness between 0 and 1
	 *  @return A RGBColorType value that closely approximates the passed color.
	 */	 
	RGBColorType fromHSL(float hue, float saturation, float lightness);
	
	
	/**
	 *	Creates an RGBColorType from a CMYK color 
	 *
	 *	@param cyan The cyan value between 0 and 1
	 *	@param magenta The magenta value between 0 and 1
	 *	@param yellow The cyan yellow between 0 and 1
	 *	@param black The black value between 0 and 1
	 *  @return A RGBColorType value that closely approximates the passed color.
	 */	 
	RGBColorType fromCMYK(float cyan, float magenta, float yellow, float black);

}


#endif //__RGBCOLOR_H__
