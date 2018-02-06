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

#include "GrayScaleColor.h"

GrayScaleColorType GrayScaleColor::fromFraction( float whiteFraction ) {
	if (whiteFraction <= 0) {
		return GRAYSCALE_BLACK;
	} else if (whiteFraction >= 1) {
		return GRAYSCALE_WHITE;
	} else {
		return GRAYSCALE_WHITE*whiteFraction;
	}
}
