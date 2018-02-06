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

#include "GrayScaleImage.h"

#pragma mark GrayScaleImage

GrayScaleImage::GrayScaleImage(
	int rows,
	int columns,
	const GrayScaleColorType* data,
	bool isFromProgramSpace )
:	LEDImage<GrayScaleColorType>( rows, columns, data, isFromProgramSpace )
{
}

GrayScaleImage::GrayScaleImage(const LEDImageBase<GrayScaleColorType>& other)
:	LEDImage<GrayScaleColorType>(other)
{
}

GrayScaleImage::~GrayScaleImage()
{
}

#pragma mark MutableGrayScaleImage

MutableGrayScaleImage::MutableGrayScaleImage( int rows, int columns )
:	MutableLEDImage<GrayScaleColorType,GRAYSCALE_BLACK,GRAYSCALE_TRANSPARENT>( rows, columns )
{
}
	
MutableGrayScaleImage::MutableGrayScaleImage(
	int rows,
	int columns,
	const GrayScaleColorType* data,
	bool isFromProgramSpace )
:	MutableLEDImage<GrayScaleColorType,GRAYSCALE_BLACK,GRAYSCALE_TRANSPARENT>( rows, columns, data, isFromProgramSpace )
{
}

MutableGrayScaleImage::MutableGrayScaleImage(const LEDImageBase<GrayScaleColorType>& other)
:	MutableLEDImage<GrayScaleColorType,GRAYSCALE_BLACK,GRAYSCALE_TRANSPARENT>(other)
{
}

MutableGrayScaleImage::~MutableGrayScaleImage()
{
}
