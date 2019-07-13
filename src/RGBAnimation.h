//     Shift Register LED Matrix Project - RGBAnimation
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
#ifndef __RGBANIMATION_H__
#define __RGBANIMATION_H__
#include "TimerAction.h"
#include "RGBLEDMatrix.h"

class RGBAnimationBase : public TimerAction {

private:

	int16_t _originX;
	int16_t _originY;
	int _sequenceIndex;
	int _maxSequenceIndex;
	RGBLEDMatrix& _matrix;

protected:

	int _rightPadSize;
	int _leftPadSize;
	int _topPadSize;
	int _bottomPadSize;
	
	virtual void action();
	virtual void draw(RGBLEDMatrix& matrix)  = 0;
	virtual void erase(RGBLEDMatrix& matrix) = 0;
	
public:

	RGBAnimationBase(
			unsigned long intervalMicros,
			RGBLEDMatrix& matrix,
			int maxSequenceIndex
		);
	
	virtual void update();	
	virtual void erase();
	virtual int16_t getOriginX() const				{ return _originX; }
	virtual int16_t getOriginY() const				{ return _originY; }
	virtual int getSequenceIndex() const			{ return _sequenceIndex; }
	
	virtual void setOriginX(int16_t originX) 		{ _originX = originX; }
	virtual void setOriginY(int16_t originY)		{ _originY = originY; }
	
	virtual void incrementSequenceIndex(void) {
		_sequenceIndex++;
		if (_sequenceIndex >= _maxSequenceIndex) {
			_sequenceIndex = 0;
		}
	}
	virtual int getNextSequenceIndex() const{
		int nextIdx = _sequenceIndex + 1;
		if (nextIdx >= _maxSequenceIndex) {
			nextIdx = 0;
		}
		
		return nextIdx;
	}

	int rows() const						{ return _matrix.rows(); }
	int columns() const						{ return _matrix.columns(); }
	
	void setRightPad( int padSize );
	void setLeftPad( int padSize );
	void setTopPad( int padSize );
	void setBottomPad( int padSize );
};

class ColorBitmapSequenceAnimation : public RGBAnimationBase {
public:
	struct Frame {
		const uint16_t *bitmap;
		int16_t bitmapWidth;
		int16_t bitmapHeight;
		int16_t x;
		int16_t y;
		unsigned long interval;
	};
private:
	const Frame* _frameArray;
	const int _frameArraySize;
	int _lastDrawnSequenceIdx;
	RGBColorType _backgroundColor;
		
protected:
	virtual void erase(RGBLEDMatrix& matrix);
	virtual void draw(RGBLEDMatrix& matrix);

public:

	ColorBitmapSequenceAnimation(
			RGBLEDMatrix& matrix,
			const Frame* frameArray,
			int	frameArraySize,
			RGBColorType backgroundColor
	);
};

class MonoBitmapSequenceAnimation : public RGBAnimationBase {
public:
	struct Frame {
		const uint8_t *bitmap;
		int16_t bitmapWidth;
		int16_t bitmapHeight;
		int16_t x;
		int16_t y;
		RGBColorType color;
		RGBColorType background;
		unsigned long interval;
	};
private:
	const Frame* _frameArray;
	const int _frameArraySize;
	int _lastDrawnSequenceIdx;
		
protected:
	virtual void erase(RGBLEDMatrix& matrix);
	virtual void draw(RGBLEDMatrix& matrix);

public:

	MonoBitmapSequenceAnimation(
			RGBLEDMatrix& matrix,
			const Frame* frameArray,
			int	frameArraySize
	);


};

#endif
