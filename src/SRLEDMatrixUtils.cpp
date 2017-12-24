//     Shift Register LED Matrix Project - Utils
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
#include <Arduino.h>
#if defined ( ESP8266 )
  #include <pgmspace.h>
#else
  #include <avr/pgmspace.h>
#endif
#include "SRLEDMatrixUtils.h"


void * SRLEDMatrixUtils::memcpy_smart(
			void * dest,
			const void * src,
			size_t n,
			bool isSrcProgMem
		)
{
	if (isSrcProgMem) {
		return memcpy_P(dest, src, n);
	} else {
		return memcpy(dest, src, n);
	}
} 

float SRLEDMatrixUtils::fmod(float x, float y) {
	return x - y * floor(x/y);
}
