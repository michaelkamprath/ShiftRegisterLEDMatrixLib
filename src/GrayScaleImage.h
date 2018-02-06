//     Shift Register LED Matrix Project - GrayScaleImage
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
#ifndef __GRAYSCALEIMAGE_H__
#define __GRAYSCALEIMAGE_H__

#include "GrayScaleColor.h"
#include "LEDImage.h"

class GrayScaleImage : public LEDImage<GrayScaleColorType> {
public:
	GrayScaleImage(
			int rows,
			int columns,
			const GrayScaleColorType* data,
			bool isFromProgramSpace = false
		);
	GrayScaleImage(const LEDImageBase<GrayScaleColorType>& other);
	virtual ~GrayScaleImage();

};

class MutableGrayScaleImage : public MutableLEDImage<GrayScaleColorType,GRAYSCALE_BLACK,GRAYSCALE_TRANSPARENT> {
public:
	MutableGrayScaleImage(int rows, int columns);
	MutableGrayScaleImage(
			int rows,
			int columns,
			const GrayScaleColorType* data,
			bool isFromProgramSpace = false
		);
	MutableGrayScaleImage(const LEDImageBase<GrayScaleColorType>& other);
	virtual ~MutableGrayScaleImage();
};


#endif //__GRAYSCALEIMAGE_H__
