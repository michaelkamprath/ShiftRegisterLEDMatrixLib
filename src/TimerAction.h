//     RGB Matrix Project - Timer Action
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

#ifndef __TIMERACTION_H__
#define __TIMERACTION_H__

class TimerAction {
private:
	unsigned long _interval;
	unsigned long  _lastLoopMicros;
	unsigned long  _actionAverageMicros;
	
	bool _isActive;
	
	unsigned long timeSinceLast(void) const {
		unsigned long curMicros = micros();
		// current micros is less than last micros. Since micros is 
		// represented by a 4 byte "long", the count will roll over at
		// about 1 hour 10 minutes.
		unsigned long timeSinceLast;
		if (curMicros < _lastLoopMicros) {
			timeSinceLast = (0xFFFFUL - _lastLoopMicros) + curMicros;
		}
		else {
			timeSinceLast = curMicros - _lastLoopMicros;
		}
		
		return timeSinceLast;
	}
protected:
  virtual void action() = 0;
public:
  TimerAction(unsigned long intervalMicros)
  		: 	_interval(intervalMicros),
  			_actionAverageMicros(0),
  			_isActive(true)
  	{
		_interval = intervalMicros;
		_lastLoopMicros = micros();
	}

	virtual void loop() {
		if (this->isActive()) {
			unsigned long delta = this->timeSinceLast();
			if ( _interval <= delta ) {
				_lastLoopMicros = micros();
				this->action();
				// at this point this->timeSinceLast() is the time it took to 
				// do the action;
				_actionAverageMicros = (_actionAverageMicros+this->timeSinceLast())/2;
			}
		}
	}

	unsigned long intervalMicros(void) const {
		return _interval;
	}
	
	void setIntervalMicros(unsigned long intervalMicros) {
		_interval = intervalMicros;
	}
	
	void setIntervalMillis(unsigned long intervalMillis) {
		_interval = intervalMillis*1000;
	}
	
	virtual void start(void) {
		_isActive = true;
		_lastLoopMicros = micros();
	}
	virtual void stop(void) {
		_isActive = false;
	}
	bool isActive(void) const {
		return _isActive;
	}
	
};

#endif //__TIMERACTION_H__
