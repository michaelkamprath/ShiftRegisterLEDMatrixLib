#include <RGBLEDMatrix.h>
#include <TimerAction.h>

class Animation : public TimerAction {
private:  
  RGBLEDMatrix* _screen;

  int _xVel;
  int _yVel;

  int _xStack[5] = {-1,-1,-1,-1,-1};
  int _yStack[5] = {-1,-1,-1,-1,-1};
  
  
protected:
  virtual void action() {
    _screen->startDrawing();
    for (int x = 0; x < _screen->rows(); ++x) {
      for (int y = 0; y < _screen->columns(); ++y ) {
        RGBColorType color = 0;
        if ( x == _xStack[4] && y == _yStack[4] ) {
          color = DARK_BLUE_COLOR;
        }
        if ( x == _xStack[3] && y == _yStack[3] ) {
#if TWELVE_BIT_COLOR
          color = 0x0009;
#else
          color = B00000010;
#endif
        }
        if ( x == _xStack[2] && y == _yStack[2] ) {
          color = BLUE_COLOR;
        }
        if ( x == _xStack[1] && y == _yStack[1] ) {
          color = MAGENTA_COLOR;
        }
        if ( x == _xStack[0] && y == _yStack[0] ) {
          color = RED_COLOR;
        }

        _screen->image().pixel(x,y) = color;
      }
    }
    _screen->stopDrawing();

    _xStack[4] = _xStack[3];
    _xStack[3] = _xStack[2];
    _xStack[2] = _xStack[1];
    _xStack[1] = _xStack[0];
    _yStack[4] = _yStack[3];
    _yStack[3] = _yStack[2];
    _yStack[2] = _yStack[1];
    _yStack[1] = _yStack[0];

    
    if ( _xStack[0] == 0 && _xVel <= 0 ) {
     _xVel = random(1,3) - 1;
     _yVel = random(0,3) - 1;
    } else if ( _xStack[0] == _screen->rows()-1 && _xVel >= 0 ) {
     _xVel = random(0,2) - 1;      
     _yVel = random(0,3) - 1;
    }

    if ( _yStack[0] == 0  && _yVel == -1) {
      _yVel = random(1,3) - 1;
    } else if ( _yStack[0] == _screen->columns()-1 && _yVel == 1) {
      _yVel = random(0,2) - 1;      
    }
    

    _xStack[0] += _xVel;
    _yStack[0] += _yVel;
  }

public:

  Animation(RGBLEDMatrix* pScreen )
    : TimerAction(100000),
      _screen(pScreen)
    {
      _xStack[0] = random(_screen->rows());
      _yStack[0] = random(_screen->columns());
      _xVel = 1;
      _yVel = 0;
      
    }

  
};

RGBLEDMatrix leds(10,10);
Animation ani(&leds);

void setup() {
  leds.setup();
  leds.startScanning();
}

void loop() {  
  leds.loop();
  ani.loop();
}
