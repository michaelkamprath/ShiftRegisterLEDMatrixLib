//     RGB Matrix Project - RGBAnimation
//     Copyright (C) 2017 Michael Kamprath
//
//     This file is part of RGB Matrix Project.
// 
//     RGB Matrix Project is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     RGB Matrix Project is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with RGB Matrix Project.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __RGBANIMATION_H__
#define __RGBANIMATION_H__
#include "TimerAction.h"
#include "RGBImage.h"
#include "RGBLEDMatrix.h"

class Glyph;

class RGBAnimationBase : public TimerAction {

private:

	int _originRow;
	int _originColumn;
	int _sequenceIndex;
	int _maxSequenceIndex;
	RGBLEDMatrix& _matrix;

	int _rightPadSize;
	ColorType _rightPadColor;
	int _leftPadSize;
	ColorType _leftPadColor;
	int _topPadSize;
	ColorType _topPadColor;
	int _bottomPadSize;
	ColorType _bottomPadColor;
	
protected:
	virtual void action();
	virtual void draw( MutableRGBImage& buffer)  = 0;
	
public:

	RGBAnimationBase(
			unsigned long intervalMicros,
			RGBLEDMatrix& matrix,
			int maxSequenceIndex
		);
	
	virtual void update();	
	virtual int getOriginRow() const				{ return _originRow; }
	virtual int getOriginColumn() const				{ return _originColumn; }
	virtual int getSequenceIndex() const			{ return _sequenceIndex; }
	
	virtual void setOriginRow(int originRow) 		{ _originRow = originRow; }
	virtual void setOriginColumn(int originColumn)	{ _originColumn = originColumn; }
	
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
	
	void setRightPad( int padSize, ColorType padColor = BLACK_COLOR );
	void setLeftPad( int padSize, ColorType padColor = BLACK_COLOR );
	void setTopPad( int padSize, ColorType padColor = BLACK_COLOR );
	void setBottomPad( int padSize, ColorType padColor = BLACK_COLOR );
};

class GlyphSequenceAnimation : public RGBAnimationBase {
public:
	struct Frame {
		const Glyph* glyph;
		int row;
		int column;
		unsigned long interval;
		ColorType foreground;
		ColorType background;
	};

private:
	const Frame* _frameArray;
	const int _frameArraySize;
	int _lastDrawnSequenceIdx;
		
protected:
	virtual void draw( MutableRGBImage& buffer);

public:

	GlyphSequenceAnimation(
			RGBLEDMatrix& matrix,
			const Frame* frameArray,
			int	frameArraySize
	);
};

class ImageSequenceAnimation : public RGBAnimationBase {
public:
	struct Frame {
		const RGBImageBase* image;
		int row;
		int column;
		unsigned long interval;
	};

private:
	const Frame* _frameArray;
	const int _frameArraySize;
	int _lastDrawnSequenceIdx;
		
protected:
	virtual void draw( MutableRGBImage& buffer);

public:

	ImageSequenceAnimation(
			RGBLEDMatrix& matrix,
			const ImageSequenceAnimation::Frame* frameArray,
			int	frameArraySize
	);
};


#endif
