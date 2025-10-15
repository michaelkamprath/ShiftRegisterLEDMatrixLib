#include <Arduino.h>
#include <RGBLEDMatrix.h>
#include <TimerAction.h>

// See notes in the 8x8 dot chaser: the Uno R4's hardware RNG is slow, so we only
// switch to a lightweight software generator on that architecture.
#if defined(ARDUINO_ARCH_RENESAS)
#include "DotChaserRng.h"
static unsigned int randomIndex(unsigned int limit) {
  return DotChaserRng::nextIndex(limit);
}
static int randomStep(void) {
  return DotChaserRng::nextNonZeroStep();
}
#else
static unsigned int randomIndex(unsigned int limit) {
  if (limit == 0u) {
    return 0u;
  }
  return static_cast<unsigned int>(random(limit));
}
static int randomStep(void) {
  int step = 0;
  while (step == 0) {
    step = random(-1, 2);
  }
  return step;
}
#endif

class Animation : public TimerAction {
private:
  RGBLEDMatrix* _screen;

  int _xVel;
  int _yVel;

  unsigned int _xStack[5] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
  unsigned int _yStack[5] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};

protected:
  virtual void action() {
    _screen->startDrawing();
    for (unsigned int x = 0; x < _screen->rows(); ++x) {
      for (unsigned int y = 0; y < _screen->columns(); ++y ) {
        RGBColorType color = 0;
        if ( x == _xStack[4] && y == _yStack[4] ) {
          color = DARK_BLUE_COLOR;
        }
        if ( x == _xStack[3] && y == _yStack[3] ) {
          color = 0x0007;
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

        _screen->writePixel(y,x, color);
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
     _xVel = 1;
     _yVel = randomStep();
    } else if ( _xStack[0] == _screen->rows()-1 && _xVel >= 0 ) {
     _xVel = -1;
     _yVel = randomStep();
    }

    if ( _yStack[0] == 0  && _yVel == -1) {
      _yVel = 1;
      _xVel = randomStep();
    } else if ( _yStack[0] == _screen->columns()-1 && _yVel == 1) {
      _yVel = -1;
      _xVel = randomStep();
    }

    _xStack[0] += _xVel;
    _yStack[0] += _yVel;
  }

public:

  Animation(RGBLEDMatrix* pScreen )
    : TimerAction(100000),
      _screen(pScreen)
    {
      this->randomize();
    }

  void randomize() {
    for (unsigned int i = 0; i < 5; ++i) {
      _xStack[i] = 0xFFFF;
      _yStack[i] = 0xFFFF;
    }
    _xStack[0] = randomIndex(_screen->rows());
    _yStack[0] = randomIndex(_screen->columns());
    _xVel = randomStep();
    _yVel = randomStep();
  }
};

RGBLEDMatrix leds(16,16, RGBLEDMatrix::RGB_GROUPS_CPRG8, HIGH, LOW, 3);
Animation ani(&leds);

void setup() {
#if defined(ARDUINO_ARCH_RENESAS)
  DotChaserRng::seed(micros());
#else
  randomSeed(micros());
#endif
  ani.randomize();
  leds.setup();
  leds.startScanning();
}

void loop() {
  leds.loop();
  ani.loop();
}
