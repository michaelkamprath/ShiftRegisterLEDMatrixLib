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
#include "RGBLEDMatrix.h"

#pragma mark - RGBAnimationBase

RGBAnimationBase::RGBAnimationBase(
			unsigned long intervalMicros,
			RGBLEDMatrix& matrix,
			int maxSequenceIndex
	) :	TimerAction(intervalMicros),
		_originX(0),
		_originY(0),
		_sequenceIndex(0),
		_maxSequenceIndex(maxSequenceIndex),
		_matrix(matrix),
		_rightPadSize(0),
		_leftPadSize(0),
		_topPadSize(0),
		_bottomPadSize(0)
{

}


void RGBAnimationBase::action() 
{	
	this->incrementSequenceIndex();
	this->update();
}

void RGBAnimationBase::erase() {
	_matrix.startDrawing();
	this->erase(_matrix);
	_matrix.stopDrawing();
}

void RGBAnimationBase::update() {
	_matrix.startDrawing();
	this->draw(_matrix);
	_matrix.stopDrawing();
}

void RGBAnimationBase::setRightPad( int padSize ) {
	_rightPadSize = padSize;
}
void RGBAnimationBase::setLeftPad( int padSize ) {
	_leftPadSize = padSize;
}
void RGBAnimationBase::setTopPad( int padSize ) {
	_topPadSize = padSize;
}
void RGBAnimationBase::setBottomPad( int padSize ) {
	_bottomPadSize = padSize;
}

#pragma mark - ColorBitmapSequenceAnimation

ColorBitmapSequenceAnimation::ColorBitmapSequenceAnimation(
			RGBLEDMatrix& matrix,
			const Frame* frameArray,
			int	frameArraySize,
			RGBColorType backgroundColor
	) :	RGBAnimationBase(1000,matrix,frameArraySize),
		_frameArray(frameArray),
		_frameArraySize(frameArraySize),
		_lastDrawnSequenceIdx( 0 ),
		_backgroundColor(backgroundColor)
{
}

void ColorBitmapSequenceAnimation::erase(RGBLEDMatrix& matrix) {
	int idx = this->getSequenceIndex();

	matrix.writeFillRect(
		this->getOriginX() - _leftPadSize,
		this->getOriginY() - _topPadSize,
		this->columns() + _leftPadSize + _rightPadSize,
		this->rows()  + _topPadSize + _bottomPadSize,
		_backgroundColor
	);
}
	
void ColorBitmapSequenceAnimation::draw(RGBLEDMatrix& matrix)
{
	int idx = this->getSequenceIndex();

	matrix.writeFillRect(
		this->getOriginX(),
		this->getOriginY(),
		this->columns(),
		this->rows(),
		_backgroundColor
	);
			
	matrix.drawRGBBitmap(
		this->getOriginX() + _frameArray[idx].x,
		this->getOriginY() + _frameArray[idx].y,
		_frameArray[idx].bitmap,
		_frameArray[idx].bitmapWidth,
		_frameArray[idx].bitmapHeight
	);

	// this function may be called multiple times in a given interval. Only
	// 
	if ( idx != _lastDrawnSequenceIdx ) {
		this->setIntervalMillis(_frameArray[idx].interval);
	}
	_lastDrawnSequenceIdx = idx;
}

#pragma mark - MonoBitmapSequenceAnimation

MonoBitmapSequenceAnimation::MonoBitmapSequenceAnimation(
			RGBLEDMatrix& matrix,
			const Frame* frameArray,
			int	frameArraySize
	) :	RGBAnimationBase(1000,matrix,frameArraySize),
		_frameArray(frameArray),
		_frameArraySize(frameArraySize),
		_lastDrawnSequenceIdx( 0 )
{
}

void MonoBitmapSequenceAnimation::erase(RGBLEDMatrix& matrix) {
	int idx = this->getSequenceIndex();

	matrix.writeFillRect(
		this->getOriginX() - _leftPadSize,
		this->getOriginY() - _topPadSize,
		this->columns() + _leftPadSize + _rightPadSize,
		this->rows()  + _topPadSize + _bottomPadSize,
		_frameArray[idx].background
	);
}

void MonoBitmapSequenceAnimation::draw(RGBLEDMatrix& matrix)
{
	int idx = this->getSequenceIndex();

	matrix.writeFillRect(
		this->getOriginX(),
		this->getOriginY(),
		this->columns(),
		this->rows(),
		_frameArray[idx].background
	);
			
	matrix.drawBitmap(
		this->getOriginX() + _frameArray[idx].x,
		this->getOriginY() + _frameArray[idx].y,
		_frameArray[idx].bitmap,
		_frameArray[idx].bitmapWidth,
		_frameArray[idx].bitmapHeight,
		_frameArray[idx].color,
		_frameArray[idx].background
	);

	// this function may be called multiple times in a given interval. Only
	// 
	if ( idx != _lastDrawnSequenceIdx ) {
		this->setIntervalMillis(_frameArray[idx].interval);
	}
	_lastDrawnSequenceIdx = idx;
}


