/***
 * Space Invaders Animation Sequence
 * 
 * Cyclces through the classic images of attacking aliens.
 * 
 * This animations was heavily inspired by: 
 *     https://learn.adafruit.com/trinket-slash-gemma-space-invader-pendant/animation
 * 
 */
#include <RGBLEDMatrix.h>
#include <TimerAction.h>
#include <RGBAnimation.h>
#include <RGBAnimationSequence.h>

RGBLEDMatrix leds(8, 8);

PROGMEM const uint8_t invader1A[8] = {
	B00011000, 
    B00111100,
    B01111110,
    B11011011,
    B11111111,
    B00100100,
    B01011010,
    B10100101
};

PROGMEM const uint8_t invader1B[8] = {
    B00011000,
    B00111100,
    B01111110,
    B11011011,
    B11111111,
    B00100100,
    B01011010,
    B01000010
};


PROGMEM const uint8_t invader2A[8] = {
    B00000000,
    B00111100,
    B01111110,
    B11011011,
    B11011011,
    B01111110,
    B00100100,
    B11000011
};

PROGMEM const uint8_t invader2B[8] = {
    B00111100,
    B01111110,
    B11011011,
    B11011011,
    B01111110,
    B00100100,
    B00100100,
    B00100100
};

PROGMEM const uint8_t invader3A[8] = {
    B00100100,
    B00100100,
    B01111110,
    B11011011,
    B11111111,
    B11111111,
    B10100101,
    B00100100
};

PROGMEM const uint8_t invader3B[8] = {
    B00100100,
    B10100101,
    B11111111,
    B11011011,
    B11111111,
    B01111110,
    B00100100,
    B01000010
};

PROGMEM const uint8_t invader4A[8] = {
    B00111100,
    B01111110,
    B00110011,
    B01111110,
    B00111100,
    B00000000,
    B00001000,
    B00000000
};
 
PROGMEM const uint8_t invader4B[8] = {
    B00111100,
    B01111110,
    B10011001,
    B01111110,
    B00111100,
    B00000000,
    B00001000,
    B00001000
};
 
PROGMEM const uint8_t invader4C[8] = {
    B00111100,
    B01111110,
    B11001100,
    B01111110,
    B00111100,
    B00000000,
    B00000000,
    B00001000
};
 
PROGMEM const uint8_t invader4D[8] = {
    B00111100,
    B01111110,
    B01100110,
    B01111110,
    B00111100,
    B00000000,
    B00000000,
    B00000000
};

// first
MonoBitmapSequenceAnimation::Frame ani1[2] = {
  {
    invader1A,
    8, 8,
    0, 0,
    DARK_GREEN_COLOR,
    BLACK_COLOR,
    500
  },
  {
    invader1B,
    8, 8,
    0, 0,
    DARK_GREEN_COLOR,
    BLACK_COLOR,
    500
  },
};

// second
MonoBitmapSequenceAnimation::Frame ani2[2] = {
  {
    invader2A,
    8, 8,
    0, 0,
    RED_COLOR,
    BLACK_COLOR,
    500
  },
  {
    invader2B,
    8, 8,
    0, 0,
    RED_COLOR,
    BLACK_COLOR,
    500
  },
};
 
// third
MonoBitmapSequenceAnimation::Frame ani3[2] = {
  {
    invader3A,
    8, 8,
    0, 0,
    ORANGE_COLOR,
    BLACK_COLOR,
    500
  },
  {
	invader3B,
    8, 8,
    0, 0,
    ORANGE_COLOR,
    BLACK_COLOR,
    500
  },
};

// fourth
MonoBitmapSequenceAnimation::Frame ani4[4] = {
  {
    invader4A,
    8, 8,
    0, 0,
    BLUE_COLOR,
    BLACK_COLOR,
    250
  },
  {
    invader4B,
    8, 8,
    0, 0,
    BLUE_COLOR,
    BLACK_COLOR,
    250
  },
  {
    invader4C,
    8, 8,
    0, 0,
    BLUE_COLOR,
    BLACK_COLOR,
    250
  },
  {
    invader4D,
    8, 8,
    0, 0,
    BLUE_COLOR,
    BLACK_COLOR,
    250
  }
};

// animation
const int ANIMATION_ITEMS_SIZE = 9;
RGBAnimationSequence::AnimationItem aniItems[ANIMATION_ITEMS_SIZE] = {
  {
    new MonoBitmapSequenceAnimation( leds, ani1, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_LEFT,
    2
  },
  {
    new MonoBitmapSequenceAnimation( leds, ani1, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_DOWN,
    2
  },
  {
    new MonoBitmapSequenceAnimation( leds, ani2, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_RIGHT,
    2
  },
  {
    new MonoBitmapSequenceAnimation( leds, ani2, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_DOWN,
    2
  },
  {
    new MonoBitmapSequenceAnimation( leds, ani3, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_LEFT,
    2
  },
  {
    new MonoBitmapSequenceAnimation( leds, ani3, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_DOWN,
    2
  },
  {
    new MonoBitmapSequenceAnimation( leds, ani4, 4 ),
    6000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_UP,
    2
  },
  {
    new MonoBitmapSequenceAnimation( leds, ani3, 2 ),
    0,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_UP,
    2
  },
  {
    new MonoBitmapSequenceAnimation( leds, ani2, 2 ),
    0,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_UP,
    2
  },
  
};

RGBAnimationSequence animation( aniItems, ANIMATION_ITEMS_SIZE);

void setup() {
    leds.setup();
    leds.startScanning();
}

void loop() {  
  leds.loop();
  animation.loop();
}

