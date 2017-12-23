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

#include <Arduino.h>
#include "RGBAnimation.h"
#include "Glyph.h"
#include "RGBLEDMatrix.h"

#pragma mark - RGBAnimationBase

RGBAnimationBase::RGBAnimationBase(
			unsigned long intervalMicros,
			RGBLEDMatrix& matrix,
			int maxSequenceIndex
	) :	TimerAction(intervalMicros),
		_originRow(0),
		_originColumn(0),
		_sequenceIndex(0),
		_maxSequenceIndex(maxSequenceIndex),
		_matrix(matrix),
		_rightPadSize(0),
		_rightPadColor(BLACK_COLOR),
		_leftPadSize(0),
		_leftPadColor(BLACK_COLOR),
		_topPadSize(0),
		_topPadColor(BLACK_COLOR),
		_bottomPadSize(0),
		_bottomPadColor(BLACK_COLOR)
{

}


void RGBAnimationBase::action() 
{	
	this->incrementSequenceIndex();
	this->update();
}

void RGBAnimationBase::update() {
	_matrix.startDrawing();
	this->draw(_matrix.image());
	if (_rightPadSize > 0) {
		int imgRightColumn = this->getOriginColumn() + _matrix.image().columns();
		for (	int x = imgRightColumn >= 0 ? imgRightColumn : 0;
				(x < _matrix.image().columns())&&(x < imgRightColumn + _rightPadSize);
				x++ )
		{
			for (int y = 0; y < _matrix.image().rows(); y++) {
				_matrix.image().pixel(y,x) = _rightPadColor;
			}
		}
	}
	if (_leftPadSize > 0) {
		int imgLeftColumn = this->getOriginColumn();
		for (	int x = imgLeftColumn - _leftPadSize >= 0 ? imgLeftColumn - _leftPadSize : 0;
				(x < _matrix.image().columns())&&(x < imgLeftColumn);
				x++ )
		{
			for (int y = 0; y < _matrix.image().rows(); y++) {
				_matrix.image().pixel(y,x) = _leftPadColor;
			}
		}
	}
	if (_topPadSize > 0) {
		int imgTopRow = this->getOriginRow();
		for (	int y = imgTopRow - _topPadSize >= 0 ? imgTopRow - _topPadSize : 0;
				(y < _matrix.image().rows())&&(y < imgTopRow);
				y++ )
		{
			for (int x = 0; x < _matrix.image().columns(); x++) {
				_matrix.image().pixel(y,x) = _topPadColor;
			}
		}
	}
	if (_bottomPadSize > 0) {
		int imgBottomRow = this->getOriginRow() + _matrix.image().rows();
		for (	int y = imgBottomRow >= 0 ? imgBottomRow : 0;
				(y < _matrix.image().rows())&&(y < imgBottomRow + _bottomPadSize);
				y++ )
		{
			for (int x = 0; x < _matrix.image().columns(); x++) {
				_matrix.image().pixel(y,x) = _bottomPadColor;
			}
		}
	}

	_matrix.stopDrawing();
}

void RGBAnimationBase::setRightPad( int padSize, RGBColorType padColor ) {
	_rightPadSize = padSize;
	_rightPadColor = padColor;
}
void RGBAnimationBase::setLeftPad( int padSize, RGBColorType padColor ) {
	_leftPadSize = padSize;
	_leftPadColor = padColor;
}
void RGBAnimationBase::setTopPad( int padSize, RGBColorType padColor ) {
	_topPadSize = padSize;
	_topPadColor = padColor;
}
void RGBAnimationBase::setBottomPad( int padSize, RGBColorType padColor ) {
	_bottomPadSize = padSize;
	_bottomPadColor = padColor;
}

#pragma mark - GlyphSequenceAnimation

GlyphSequenceAnimation::GlyphSequenceAnimation(
			RGBLEDMatrix& matrix,
			const Frame* frameArray,
			int	frameArraySize
	) :	RGBAnimationBase(1000,matrix,frameArraySize),
		_frameArray(frameArray),
		_frameArraySize(frameArraySize),
		_lastDrawnSequenceIdx( 0 )
{
}
		
void GlyphSequenceAnimation::draw( MutableRGBImage& buffer)
{
	int idx = this->getSequenceIndex();

	buffer.drawRectangle(
		this->getOriginRow(),
		this->getOriginColumn(),
		this->getOriginRow() + buffer.rows() - 1,
		this->getOriginColumn() + buffer.columns() - 1,
		_frameArray[idx].background,
		true
	);
			
	buffer.drawGlyph(
		*_frameArray[idx].glyph,
		this->getOriginRow() + _frameArray[idx].row,
		this->getOriginColumn() + _frameArray[idx].column,
		_frameArray[idx].foreground,
		_frameArray[idx].background
	);

	// this function may be called multiple times in a given interval. Only
	// 
	if ( idx != _lastDrawnSequenceIdx ) {
		this->setIntervalMillis(_frameArray[idx].interval);
	}
	_lastDrawnSequenceIdx = idx;
}


#pragma mark - ImageSequenceAnimation

ImageSequenceAnimation::ImageSequenceAnimation(
			RGBLEDMatrix& matrix,
			const ImageSequenceAnimation::Frame* frameArray,
			int	frameArraySize
	) :	RGBAnimationBase(1000,matrix,frameArraySize),
		_frameArray(frameArray),
		_frameArraySize(frameArraySize),
		_lastDrawnSequenceIdx( 0 )
{
}
		

void ImageSequenceAnimation::draw( MutableRGBImage& buffer)
{
	int idx = this->getSequenceIndex();

	buffer.drawRectangle(
		this->getOriginRow(),
		this->getOriginColumn(),
		this->getOriginRow() + buffer.rows() - 1,
		this->getOriginColumn() + buffer.columns() - 1,
		BLACK_COLOR,
		true
	);

	buffer.placeImageAt(
		*_frameArray[idx].image,
		this->getOriginRow() + _frameArray[idx].row,
		this->getOriginColumn() + _frameArray[idx].column
	);

	// this function may be called multiple times in a given interval. Only
	// 
	if ( idx != _lastDrawnSequenceIdx ) {
		this->setIntervalMillis(_frameArray[idx].interval);
	}
	_lastDrawnSequenceIdx = idx;
}

