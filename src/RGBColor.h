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

/***
RGB Color

RGB color is a 16-bit color fit into a uint16_t. The red and blue components each get 5 bits
of data, and green gets 6. The bit layout for the uint16_t is as follows:

	Bits   0   4   8  12  
		   |---|---|---|---
 		   RRRRRGGGGGGBBBBB
 
Even though the color information is encoded into a 16 bit format, some microcontrollers
may only leverage effectively 9 bits of information, or three bits of information per color. 
This is not done for memory space savings , but instead because some microcontrollers 
cannot scan through the LED rows fast enough to enable 16 bit color. When in 6-bit mode,
these are the bits considered when determining the color level:

	Bits   0   4   8  12  
		   |---|---|---|---
 		   R-R-RG-G--GB-B-B
 		   
Or, the following big masks are used:

	RED_MASK_6BIT 	= 0xA800
	GREEN_MASK_6BIT	= 0x0540
	BLUE_MASK_6BIT	= 0x0015
***/

#ifndef SIXTEEN_BIT_COLOR
#define SIXTEEN_BIT_COLOR (defined(__arm__)&& defined(TEENSYDUINO)) \
							||defined(__AVR_ATmega2560__) \
							||defined( ESP32 ) \
							||defined(ARDUINO_SAMD_ZERO) \
							||defined(_SAM3XA_)
							
// You may wish to add the following platforms to the set that support 12-bit color
//
//							||defined( ESP8266 ) 
// 							||defined(__AVR_ATmega1284__) 
// 							||defined(__AVR_ATmega1284P__) 

#endif

typedef uint16_t RGBColorType;

const RGBColorType RED_COLOR_MASK = 0xF800;
const RGBColorType GREEN_COLOR_MASK = 0x07E0;
const RGBColorType BLUE_COLOR_MASK = 0x001F;


const RGBColorType AQUA_COLOR = GREEN_COLOR_MASK|BLUE_COLOR_MASK;
const RGBColorType BLACK_COLOR = 0;
const RGBColorType BLUE_COLOR = BLUE_COLOR_MASK;
const RGBColorType BROWN_COLOR = 0xA145;
const RGBColorType CORAL_COLOR = 0xF8E5;
const RGBColorType DARK_BLUE_COLOR = 0x0004;
const RGBColorType DARK_GRAY_COLOR = 0x821;
const RGBColorType DARK_GREEN_COLOR = 0x0020;
const RGBColorType DARK_RED_COLOR = 0x0800;
const RGBColorType GRAY_COLOR = 0x39E7;
const RGBColorType GREEN_COLOR = GREEN_COLOR_MASK;
const RGBColorType LIME_COLOR = 0x1983;
const RGBColorType MAGENTA_COLOR = 0xF81F;
const RGBColorType ORANGE_COLOR = 0xF8E0;
const RGBColorType PINK_COLOR = 0xF8B2;
const RGBColorType PURPLE_COLOR = 0x3807;
const RGBColorType RED_COLOR = RED_COLOR_MASK;
const RGBColorType SKY_BLUE_COLOR = 0x867D;
const RGBColorType SLATE_BLUE_COLOR = 0x6ADB;
const RGBColorType TURQUOISE_COLOR = 0x471A;
const RGBColorType VIOLET_COLOR = 0x901A;
const RGBColorType WHITE_COLOR = 0xFFFF;
const RGBColorType YELLOW_COLOR = RED_COLOR_MASK|GREEN_COLOR_MASK;

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
