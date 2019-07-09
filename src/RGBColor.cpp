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

#include <math.h>
#include "RGBColor.h"
#include "SRLEDMatrixUtils.h"


#define RED_SCALE_VALUE 0x08
#define GREEN_SCALE_VALUE 0x04
#define BLUE_SCALE_VALUE 0x08

#define RED_BIT_SHIFT 11
#define GREEN_BIT_SHIFT 5
#define BLUE_BIT_SHIFT 0

#define RED_MAX_VALUE 0x1F
#define GREEN_MAX_VALUE 0x3F
#define BLUE_MAX_VALUE 0x1F

RGBColorType RGBColor::fromRGB(int red, int green, int blue) {
	RGBColorType r = red/RED_SCALE_VALUE;
	RGBColorType g = green/GREEN_SCALE_VALUE;
	RGBColorType b = blue/BLUE_SCALE_VALUE;

	RGBColorType color = (r<<RED_BIT_SHIFT)|(g<<GREEN_BIT_SHIFT)|(b);
	
	return color;
}

RGBColorType RGBColor::fromHSV(float hue, float saturation, float value) {
	float H = ( hue < 0 ? ( SRLEDMatrixUtils::fmod(hue,360) + 360.0) : SRLEDMatrixUtils::fmod(hue,360) );

	float C = value*saturation;
	float X = C*(1 - fabs(float(int(H/60.0)%2-1)) );
	float m = value - C;
	
	float r;
	float g;
	float b;
	if ( H < 60 ) {
		r = C;
		g = X;
		b = 0;
	} else if ( H < 120 ) {
		r = X;
		g = C;
		b = 0;
	} else if ( H < 180 ) {
		r = 0;
		g = C;
		b = X;
	} else if ( H < 240 ) {
		r = 0;
		g = X;
		b = C;
	} else if ( H < 300 ) {
		r = X;
		g = 0;
		b = C;
	} else {
		r = C;
		g = 0;
		b = X;
	}

	RGBColorType color = (int( (r+m)*RED_MAX_VALUE )<<RED_BIT_SHIFT)
						|(int( (g+m)*GREEN_MAX_VALUE )<<GREEN_BIT_SHIFT)
						|(int( (b+m)*BLUE_MAX_VALUE ));	
						
	return color;
}

RGBColorType RGBColor::fromHSL(float hue, float saturation, float lightness) {
	float H = ( hue < 0 ? ( SRLEDMatrixUtils::fmod(hue,360) + 360.0) : SRLEDMatrixUtils::fmod(hue,360) );

	float C = (1.0 - fabs(2.0*lightness - 1.0))*saturation;
	float X = C*(1 - fabs(float(int(H/60.0)%2-1)) );
	float m = lightness - C/2.0;

	float r;
	float g;
	float b;
	if ( H < 60 ) {
		r = C;
		g = X;
		b = 0;
	} else if ( H < 120 ) {
		r = X;
		g = C;
		b = 0;
	} else if ( H < 180 ) {
		r = 0;
		g = C;
		b = X;
	} else if ( H < 240 ) {
		r = 0;
		g = X;
		b = C;
	} else if ( H < 300 ) {
		r = X;
		g = 0;
		b = C;
	} else {
		r = C;
		g = 0;
		b = X;
	}

	RGBColorType color = (int( (r+m)*RED_MAX_VALUE )<<RED_BIT_SHIFT)
						|(int( (g+m)*GREEN_MAX_VALUE )<<GREEN_BIT_SHIFT)
						|(int( (b+m)*BLUE_MAX_VALUE ));	
						
	return color;
}

RGBColorType RGBColor::fromCMYK(float cyan, float magenta, float yellow, float black) {

	float r = (1.0-cyan)*(1.0-black);
	float g = (1.0-magenta)*(1.0-black);
	float b = (1.0-yellow)*(1.0-black);

	RGBColorType color = (int( r*RED_MAX_VALUE )<<RED_BIT_SHIFT)
						|(int( g*GREEN_MAX_VALUE )<<GREEN_BIT_SHIFT)
						|(int( b*BLUE_MAX_VALUE ));	
						
	return color;
}

