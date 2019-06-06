/***
 * Space Invaders Animation Sequence
 * 
 * Cyclces through the classic images of attacking aliens.
 * 
 * This animations was heavily inspired by: 
 *     https://learn.adafruit.com/trinket-slash-gemma-space-invader-pendant/animation
 * 
 */


#include <Glyph.h>
#include <RGBImage.h>
#include <RGBLEDMatrix.h>
#include <TimerAction.h>
#include <RGBAnimation.h>
#include <RGBAnimationSequence.h>

RGBLEDMatrix leds(8,8);

PROGMEM bool const invader1A[64] = {
    0,0,0,1,1,0,0,0, 
    0,0,1,1,1,1,0,0,
    0,1,1,1,1,1,1,0,
    1,1,0,1,1,0,1,1,
    1,1,1,1,1,1,1,1,
    0,0,1,0,0,1,0,0,
    0,1,0,1,1,0,1,0,
    1,0,1,0,0,1,0,1
};

PROGMEM bool const invader1B[64] = {
    0,0,0,1,1,0,0,0,
    0,0,1,1,1,1,0,0,
    0,1,1,1,1,1,1,0,
    1,1,0,1,1,0,1,1,
    1,1,1,1,1,1,1,1,
    0,0,1,0,0,1,0,0,
    0,1,0,1,1,0,1,0,
    0,1,0,0,0,0,1,0
};


PROGMEM bool const invader2A[64] = {
  0,0,0,0,0,0,0,0,
  0,0,1,1,1,1,0,0,
  0,1,1,1,1,1,1,0,
  1,1,0,1,1,0,1,1,
  1,1,0,1,1,0,1,1,
  0,1,1,1,1,1,1,0,
  0,0,1,0,0,1,0,0,
  1,1,0,0,0,0,1,1
};

PROGMEM bool const invader2B[64] = {
  0,0,1,1,1,1,0,0,
  0,1,1,1,1,1,1,0,
  1,1,0,1,1,0,1,1,
  1,1,0,1,1,0,1,1,
  0,1,1,1,1,1,1,0,
  0,0,1,0,0,1,0,0,
  0,0,1,0,0,1,0,0,
  0,0,1,0,0,1,0,0
};

PROGMEM bool const invader3A[64] = {
  0,0,1,0,0,1,0,0,
  0,0,1,0,0,1,0,0,
  0,1,1,1,1,1,1,0,
  1,1,0,1,1,0,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,0,1,0,0,1,0,1,
  0,0,1,0,0,1,0,0
};

PROGMEM bool const invader3B[64] = {
  0,0,1,0,0,1,0,0,
  1,0,1,0,0,1,0,1,
  1,1,1,1,1,1,1,1,
  1,1,0,1,1,0,1,1,
  1,1,1,1,1,1,1,1,
  0,1,1,1,1,1,1,0,
  0,0,1,0,0,1,0,0,
  0,1,0,0,0,0,1,0
};

PROGMEM bool const invader4A[64] = {
  0,0,1,1,1,1,0,0,
  0,1,1,1,1,1,1,0,
  0,0,1,1,0,0,1,1,
  0,1,1,1,1,1,1,0,
  0,0,1,1,1,1,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,1,0,0,0,
  0,0,0,0,0,0,0,0
};
 
PROGMEM bool const invader4B[64] = {
  0,0,1,1,1,1,0,0,
  0,1,1,1,1,1,1,0,
  1,0,0,1,1,0,0,1,
  0,1,1,1,1,1,1,0,
  0,0,1,1,1,1,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,1,0,0,0,
  0,0,0,0,1,0,0,0
};
 
PROGMEM bool const invader4C[64] = {
  0,0,1,1,1,1,0,0,
  0,1,1,1,1,1,1,0,
  1,1,0,0,1,1,0,0,
  0,1,1,1,1,1,1,0,
  0,0,1,1,1,1,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,1,0,0,0
};
 
PROGMEM bool const invader4D[64] = {
  0,0,1,1,1,1,0,0,
  0,1,1,1,1,1,1,0,
  0,1,1,0,0,1,1,0,
  0,1,1,1,1,1,1,0,
  0,0,1,1,1,1,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0
};

Glyph a1a(8, 8, invader1A, true);
Glyph a1b(8, 8, invader1B, true);
Glyph a2a(8, 8, invader2A, true);
Glyph a2b(8, 8, invader2B, true);
Glyph a3a(8, 8, invader3A, true);
Glyph a3b(8, 8, invader3B, true);
Glyph a4a(8, 8, invader4A, true);
Glyph a4b(8, 8, invader4B, true);
Glyph a4c(8, 8, invader4C, true);
Glyph a4d(8, 8, invader4D, true);

// first
GlyphSequenceAnimation::Frame ani1[2] = {
  {
    &a1a,
    0, 0,
    500,
    DARK_GREEN_COLOR,
    BLACK_COLOR
  },
  {
    &a1b,
    0, 0,
    500,
    DARK_GREEN_COLOR,
    BLACK_COLOR
  },
};

// second
GlyphSequenceAnimation::Frame ani2[2] = {
  {
    &a2a,
    0, 0,
    500,
    RED_COLOR,
    BLACK_COLOR
  },
  {
    &a2b,
    0, 0,
    500,
    RED_COLOR,
    BLACK_COLOR
  },
};
 
// third
GlyphSequenceAnimation::Frame ani3[2] = {
  {
    &a3a,
    0, 0,
    500,
    ORANGE_COLOR,
    BLACK_COLOR
  },
  {
    &a3b,
    0, 0,
    500,
    ORANGE_COLOR,
    BLACK_COLOR
  },
};

// fourth
GlyphSequenceAnimation::Frame ani4[4] = {
  {
    &a4a,
    0, 0,
    250,
    BLUE_COLOR,
    BLACK_COLOR
  },
  {
    &a4b,
    0, 0,
    250,
    BLUE_COLOR,
    BLACK_COLOR
  },
  {
    &a4c,
    0, 0,
    250,
    BLUE_COLOR,
    BLACK_COLOR
  },
  {
    &a4d,
    0, 0,
    250,
    BLUE_COLOR,
    BLACK_COLOR
  }
};

// animation
const int ANIMATION_ITEMS_SIZE = 9;
RGBAnimationSequence::AnimationItem aniItems[ANIMATION_ITEMS_SIZE] {
  {
    new GlyphSequenceAnimation( leds, ani1, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_LEFT,
    2
  },
  {
    new GlyphSequenceAnimation( leds, ani1, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_DOWN,
    2
  },
  {
    new GlyphSequenceAnimation( leds, ani2, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_RIGHT,
    2
  },
  {
    new GlyphSequenceAnimation( leds, ani2, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_DOWN,
    2
  },
  {
    new GlyphSequenceAnimation( leds, ani3, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_LEFT,
    2
  },
  {
    new GlyphSequenceAnimation( leds, ani3, 2 ),
    4000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_DOWN,
    2
  },
  {
    new GlyphSequenceAnimation( leds, ani4, 4 ),
    6000,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_UP,
    2
  },
  {
    new GlyphSequenceAnimation( leds, ani3, 2 ),
    0,
    2000,
    RGBAnimationSequence::TRANSITION_SLIDE_UP,
    2
  },
  {
    new GlyphSequenceAnimation( leds, ani2, 2 ),
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

