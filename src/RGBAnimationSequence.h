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

#ifndef __RGBANIMATIONSEQUENCE_H__
#define __RGBANIMATIONSEQUENCE_H__
#include "RGBAnimation.h"

class RGBAnimationSequence : public TimerAction {
public:
	typedef enum {
		TRANSITION_APPEAR,
		TRANSITION_SLIDE_LEFT,
		TRANSITION_SLIDE_RIGHT,
		TRANSITION_SLIDE_UP,
		TRANSITION_SLIDE_DOWN
	} TransitionType;
	
	struct AnimationItem {
		RGBAnimationBase* animation;
		unsigned long appearMillis;
		unsigned long transitionMillis;
		TransitionType transition;	
		int transitionPad;
	};

private:
	typedef enum {
		ANIMATION_START,
		ANIMATION_ITEM,
		ANIMATION_TRANSITION
	} StateType;
	
	AnimationItem* _aniArray;
	const int _aniArraySize;
	bool* _itemIsInView;
	
	StateType _curState;
	int _curItemIdx;
	int _curItemOriginX;
	int _curItemOriginY;
	int _nextItemOriginX;
	int _nextItemOriginY;
	int _transitionIntervalMillis;
	int _transitionStep;
	int _transitionItemIdx;
	
protected:
	virtual void action();
	
public:
	
	// Note that while in most other parts of this lib it is OK to reuse
	// objects in a sequence list (e.g., using the same Glyph multiple time 
	// in a GlyphAnimation object), the RGBAnimationBase objects passed in
	// the AnimationItem list here should all be distinct. The reason for that
	// is that each animation's state needs to be maintained separately.
	RGBAnimationSequence(
			AnimationItem* aniArray,
			int	aniArraySize
	);

	virtual void loop();


};

#endif
