//     Shift Register LED Matrix Project - GrayScaleColor
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
#ifndef __GRAYSCALECOLOR_H__
#define __GRAYSCALECOLOR_H__

/***
Each byte controls one mono-color LED.
	Bits   0   4
		   |---|---
		   TUUUUUXX
	T = transparent
	U = unused
	X = bit indicating gray scale
*/

typedef unsigned char GrayScaleColorType;

const GrayScaleColorType GRAYSCALE_WHITE = 0x03;
const GrayScaleColorType GRAYSCALE_BLACK = 0;
const GrayScaleColorType GRAYSCALE_LIGHT_GRAY = 0x02;
const GrayScaleColorType GRAYSCALE_DARK_GRAY = 0x01;

const GrayScaleColorType GRAYSCALE_TRANSPARENT = 0x80;

namespace GrayScaleColor {

	GrayScaleColorType fromFraction( float whiteFraction );
}

#endif //__GRAYSCALECOLOR_H__
