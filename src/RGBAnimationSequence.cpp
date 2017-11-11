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
#include "RGBAnimationSequence.h"

RGBAnimationSequence::RGBAnimationSequence(
			AnimationItem* aniArray,
			int	aniArraySize
	) :	TimerAction(1000),
		_aniArray(aniArray),
		_aniArraySize(aniArraySize),
		_itemIsInView(new bool[aniArraySize]),
		_curState(ANIMATION_START),
		_curItemIdx(0),
		_curItemOriginX(0),
		_curItemOriginY(0),
		_nextItemOriginX(0),
		_nextItemOriginY(0)
{
	memset(	_itemIsInView, 0, aniArraySize*sizeof(bool) );
}

void RGBAnimationSequence::loop()
{
	this->TimerAction::loop();
	
	for (int i = 0; i < _aniArraySize; i++) {
		if (_itemIsInView[i]) {
			_aniArray[i].animation->loop();
		}
	}	
}

void RGBAnimationSequence::action() {
	// the action here is really control which animations are visible
	// and what their origin point is. This animation doesn't draw as that is 
	// being done by the animations items in the list.

	if (_curState == ANIMATION_START) {
		RGBAnimationBase* curAni = _aniArray[_curItemIdx].animation;
		curAni->setOriginRow(0);
		curAni->setOriginColumn(0);
		_itemIsInView[0] = true;
		this->setIntervalMillis(_aniArray[_curItemIdx].appearMillis);
		
		_curState = ANIMATION_ITEM;
	}
	else if (_curState == ANIMATION_ITEM) {
		//set up transition
		if ( _aniArray[_curItemIdx].transition == TRANSITION_APPEAR ) {
			_itemIsInView[_curItemIdx] = false;
			
			_curItemIdx++;
			if (_curItemIdx >= _aniArraySize) {
				_curItemIdx = 0;
			}
			
			RGBAnimationBase* curAni = _aniArray[_curItemIdx].animation;
			curAni = _aniArray[_curItemIdx].animation;
			curAni->setOriginRow(0);
			curAni->setOriginColumn(0);
			_itemIsInView[_curItemIdx] = true;
			
			this->setIntervalMillis(_aniArray[_curItemIdx].appearMillis);
		}
		else if ( 
					(_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_LEFT)
					||(_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_RIGHT)
					||(_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_UP)
					||(_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_DOWN)
				) 
		{
			_transitionItemIdx = _curItemIdx+1;
			if (_transitionItemIdx >= _aniArraySize) {
				_transitionItemIdx = 0;
			}
			
			RGBAnimationBase* curAni = _aniArray[_curItemIdx].animation;
			RGBAnimationBase* nextAni = _aniArray[_transitionItemIdx].animation;
			
			_curItemOriginX = 0;
			_curItemOriginY = 0;
			if (_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_RIGHT ) {
				_nextItemOriginX = -(nextAni->columns() + _aniArray[_curItemIdx].transitionPad);
				curAni->setLeftPad( _aniArray[_curItemIdx].transitionPad );
			}
			else if (_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_LEFT ){
				_nextItemOriginX = curAni->columns() + _aniArray[_curItemIdx].transitionPad;
				curAni->setRightPad( _aniArray[_curItemIdx].transitionPad );
			}
			else {
				_nextItemOriginX = 0;
			}
			
			if (_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_DOWN ) {
				_nextItemOriginY = -(nextAni->rows() + _aniArray[_curItemIdx].transitionPad);
				curAni->setTopPad( _aniArray[_curItemIdx].transitionPad );
			}
			else if (_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_UP ){
				_nextItemOriginY = curAni->rows() + _aniArray[_curItemIdx].transitionPad;
				curAni->setBottomPad( _aniArray[_curItemIdx].transitionPad );
			}
			else {
				_nextItemOriginY = 0;
			}
			
			curAni->setOriginRow(_curItemOriginY);
			curAni->setOriginColumn(_curItemOriginX);
			nextAni->setOriginRow(_nextItemOriginY);
			nextAni->setOriginColumn(_nextItemOriginX);
			
			if ( _aniArray[_curItemIdx].transition == TRANSITION_SLIDE_RIGHT
				|| _aniArray[_curItemIdx].transition == TRANSITION_SLIDE_LEFT )
			{
				_transitionIntervalMillis = _aniArray[_curItemIdx].transitionMillis/abs(_nextItemOriginX);
			} else {
				_transitionIntervalMillis = _aniArray[_curItemIdx].transitionMillis/abs(_nextItemOriginY);
			}
			_transitionStep = 0;
			_itemIsInView[_transitionItemIdx] = true;
	
			
			this->setIntervalMillis(_transitionIntervalMillis);
			_curState = ANIMATION_TRANSITION;
		}
	}
	else if (_curState == ANIMATION_TRANSITION) {
		RGBAnimationBase* curAni = _aniArray[_curItemIdx].animation;
		RGBAnimationBase* nextAni = _aniArray[_transitionItemIdx].animation;
			
		if (_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_LEFT) {
			_curItemOriginX--;
			_nextItemOriginX--;
		}
		else if (_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_RIGHT ) {	
			_curItemOriginX++;
			_nextItemOriginX++;
		}
		else if (_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_DOWN ) {	
			_curItemOriginY++;
			_nextItemOriginY++;
		}
		else if (_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_UP ) {	
			_curItemOriginY--;
			_nextItemOriginY--;
		}

		curAni->setOriginRow(_curItemOriginY);
		curAni->setOriginColumn(_curItemOriginX);
		nextAni->setOriginRow(_nextItemOriginY);
		nextAni->setOriginColumn(_nextItemOriginX);
		
		curAni->update();
		nextAni->update();
		
		if ( 	(	
					(
						(_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_LEFT)
						||(_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_RIGHT)
					) &&(
						_nextItemOriginX == 0
					)
				)||(
					(
						(_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_DOWN)
						||(_aniArray[_curItemIdx].transition == TRANSITION_SLIDE_UP)
					) &&(
						_nextItemOriginY == 0
					)
				)
			) {
			curAni->setRightPad( 0 );
			curAni->setLeftPad( 0 );
			curAni->setTopPad( 0 );
			curAni->setBottomPad( 0 );
			nextAni->setRightPad( 0 );
			nextAni->setLeftPad( 0 );
			nextAni->setTopPad( 0 );
			nextAni->setBottomPad( 0 );
			_itemIsInView[_curItemIdx] = false;
			_curState = ANIMATION_ITEM;
			_curItemIdx = _transitionItemIdx;
			_transitionItemIdx = 0;
			this->setIntervalMillis(_aniArray[_curItemIdx].appearMillis);
		}
	}
}
