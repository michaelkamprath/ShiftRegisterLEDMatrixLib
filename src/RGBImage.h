//     Shift Register LED Matrix Project - RGBImage
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
#ifndef __RGBIMAGE12BIT_H__
#define __RGBIMAGE12BIT_H__

#include "RGBColor.h"
#include "LEDImage.h"
class GlyphBase;

class RGBImage : public LEDImage<RGBColorType> {
public:
	RGBImage(
			int rows,
			int columns,
			const RGBColorType* data,
			bool isFromProgramSpace = false
		);
	RGBImage(const LEDImageBase<RGBColorType>& other);
	virtual ~RGBImage();

};

class MutableRGBImage : public MutableLEDImage<RGBColorType,BLACK_COLOR,TRANSPARENT_COLOR> {
public:
	MutableRGBImage(int rows, int columns);
	MutableRGBImage(
			int rows,
			int columns,
			const RGBColorType* data,
			bool isFromProgramSpace = false
		);
	MutableRGBImage(const LEDImageBase<RGBColorType>& other);
	virtual ~MutableRGBImage();

	void drawGlyph(
			const LEDImageBase<bool>& image,
			int row,
			int column,
			RGBColorType foreground,
			RGBColorType background = TRANSPARENT_COLOR
		);
};

#endif //__RGBIMAGE_H__

